#include "Pch.hpp"
#include "Natrium-Renderer/Renderer.hpp"

#include "Natrium-Core/Logger.hpp"

#include "Natrium-Renderer/VkContext.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Na {
	static vk::SurfaceKHR createWindowSurface(Window* window)
	{
		VkSurfaceKHR surface;
		VK_CHECK(glfwCreateWindowSurface(VkContext::GetInstance(), window->native(), nullptr, &surface));
		return surface;
	}

	static vk::SurfaceFormatKHR pickSurfaceFormat(const Na::ArrayVector<vk::SurfaceFormatKHR>& formats)
	{
		for (auto it = formats.begin(); it != formats.end(); it++)
			if (it->format == vk::Format::eR8G8B8A8Uint
			&& it->colorSpace == vk::ColorSpaceKHR::eAdobergbNonlinearEXT)
				return *it;
		return formats[0];
	}

	static vk::PresentModeKHR pickPresentMode(const Na::ArrayVector<vk::PresentModeKHR>& present_modes)
	{
		for (auto it = present_modes.begin(); it != present_modes.end(); it++)
			if (*it == vk::PresentModeKHR::eMailbox)
				return *it;
		return vk::PresentModeKHR::eFifo;
	}

	static vk::Extent2D pickResolution(const vk::SurfaceCapabilitiesKHR& capabilities, u32 window_width, u32 window_height)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
			return capabilities.currentExtent;

		return {
			std::clamp<u32>(window_width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp<u32>(window_height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
		};
	}

	Renderer::Renderer(Window& window)
	: m_Window(&window),
	m_PipelineHandle(NA_INVALID_HANDLE)
	{
		m_Config = RendererConfig{
			.max_frames_in_flight = 2,
			.anisotropy_enabled = true,
			.max_anisotropy = VkContext::GetPhysicalDevice().getProperties().limits.maxSamplerAnisotropy
		};

		_create_window_surface();
		_create_swapchain();
		_create_image_views();
		_create_depth_buffer();
		_create_render_pass();
		_create_framebuffers();
		_create_command_objects();
		_create_sync_objects();
	}

	void Renderer::destroy(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		for (auto& in_flight : m_Sync.in_flight)
			if (in_flight)
				logical_device.destroyFence(in_flight);

		for (auto& render_finished : m_Sync.render_finished)
			if (render_finished)
				logical_device.destroySemaphore(render_finished);

		for (auto& image_available : m_Sync.image_available)
			if (image_available)
				logical_device.destroySemaphore(image_available);

		if (m_SingleTimeCmdPool)
			logical_device.destroyCommandPool(m_SingleTimeCmdPool);

		if (m_GraphicsCmdPool)
			logical_device.destroyCommandPool(m_GraphicsCmdPool);

		for (auto& framebuffer : m_Framebuffers)
			if (framebuffer)
				logical_device.destroyFramebuffer(framebuffer);

		if (m_RenderPass)
			logical_device.destroyRenderPass(m_RenderPass);

		if (m_DepthImage.img)
			m_DepthImage.destroy();

		if (m_DepthImageView)
			logical_device.destroyImageView(m_DepthImageView);

		for (auto& img_view : m_ImageViews)
			if (img_view)
				logical_device.destroyImageView(img_view);

		if (m_Swapchain)
			logical_device.destroySwapchainKHR(m_Swapchain);

		if (m_Surface)
			VkContext::GetInstance().destroySurfaceKHR(m_Surface);
	}

	void Renderer::clear(const glm::vec4& color)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();
		vk::CommandBuffer& cmd_buffer = m_GraphicsCmdBuffers[m_CurrentFrame];

		if (m_HasResized)
			_recreate_swapchain();
		
		(void)logical_device.waitForFences(
			1, &m_Sync.in_flight[m_CurrentFrame],
			VK_TRUE, // wait all
			UINT64_MAX // timeout
		);

		try {
			(void)logical_device.acquireNextImageKHR(
				m_Swapchain,
				UINT64_MAX, // timeout
				m_Sync.image_available[m_CurrentFrame],
				nullptr,
				&m_ImageIndex
			);
		} catch (const vk::OutOfDateKHRError& err)
		{
			(void)err;
			_recreate_swapchain();
		}

		(void)logical_device.resetFences(1, &m_Sync.in_flight[m_CurrentFrame]);
		cmd_buffer.reset();

		vk::CommandBufferBeginInfo begin_info;
		cmd_buffer.begin(begin_info);

		std::array<vk::ClearValue, 2> clear_values{};
		clear_values[0].color = vk::ClearColorValue{ color.r, color.g, color.g, color.a };
		clear_values[1].depthStencil = { { 1.0f, 0 } };

		vk::RenderPassBeginInfo render_pass_info;

		render_pass_info.renderPass = m_RenderPass;
		render_pass_info.framebuffer = m_Framebuffers[m_ImageIndex];

		render_pass_info.renderArea.offset = {{ 0, 0 }};
		render_pass_info.renderArea.extent = m_Extent;

		render_pass_info.clearValueCount = (u32)clear_values.size();
		render_pass_info.pClearValues = clear_values.data();

		cmd_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
		if (m_PipelineHandle != NA_INVALID_HANDLE)
		{
			PipelineData& pipeline = VkContext::GetPipelinePool()[m_PipelineHandle];
			cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline);

			if (!pipeline.descriptor_sets.empty())
				cmd_buffer.bindDescriptorSets(
					vk::PipelineBindPoint::eGraphics,
					pipeline.layout,
					0, // first set
					1, &pipeline.descriptor_sets[m_CurrentFrame],
					0, nullptr // dynamic offsets
				);
		}

		cmd_buffer.setViewport(0, 1, &m_Viewport);
		cmd_buffer.setScissor(0, 1, &m_Scissor);
	}

	void Renderer::present(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();
		vk::CommandBuffer& cmd_buffer = m_GraphicsCmdBuffers[m_CurrentFrame];

		cmd_buffer.endRenderPass();
		cmd_buffer.end();

		vk::SubmitInfo submit_info;

		vk::Semaphore wait_semaphores[] = { m_Sync.image_available[m_CurrentFrame] };
		vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

		submit_info.waitSemaphoreCount = sizeof(wait_semaphores) / sizeof(vk::Semaphore);
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;

		vk::Semaphore signal_semaphores[] = { m_Sync.render_finished[m_CurrentFrame] };
		submit_info.signalSemaphoreCount = sizeof(signal_semaphores) / sizeof(VkSemaphore);
		submit_info.pSignalSemaphores = signal_semaphores;

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &cmd_buffer;

		(void)VkContext::GetGraphicsQueue().submit(1, &submit_info, m_Sync.in_flight[m_CurrentFrame]);

		vk::PresentInfoKHR present_info;
		present_info.waitSemaphoreCount = submit_info.waitSemaphoreCount;
		present_info.pWaitSemaphores = signal_semaphores;

		present_info.swapchainCount = 1;
		present_info.pSwapchains = &m_Swapchain;
		present_info.pImageIndices = &m_ImageIndex;

		try {
			vk::Result result = VkContext::GetGraphicsQueue().presentKHR(present_info);
			if (result == vk::Result::eSuboptimalKHR || m_HasResized)
				_recreate_swapchain();
		} catch (const vk::OutOfDateKHRError& err)
		{
			(void)err;
			_recreate_swapchain();
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % m_Config.max_frames_in_flight;
	}

	void Renderer::update_size(void)
	{
		m_HasResized = true;
	}

	void Renderer::_create_window_surface(void)
	{
		m_HasResized = false;

		m_Surface = createWindowSurface(m_Window);
		m_QueueIndices = QueueFamilyIndices::Get(VkContext::GetPhysicalDevice(), m_Surface);
		if (!m_QueueIndices)
			throw std::runtime_error("Queue indices is incomplete!");

		m_Width = m_Window->width();
		m_Height = m_Window->height();

		m_Viewport.x = 0.0f;
		m_Viewport.y = 0.0f;
		m_Viewport.width = (float)m_Width;
		m_Viewport.height = (float)m_Height;
		m_Viewport.minDepth = 0.0f;
		m_Viewport.maxDepth = 1.0f;

		m_Scissor.offset.x = 0;
		m_Scissor.offset.y = 0;
		m_Scissor.extent.width = m_Width;
		m_Scissor.extent.height = m_Height;
	}

	void Renderer::_create_swapchain(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		SurfaceSupport support = SurfaceSupport::Get(m_Surface, VkContext::GetPhysicalDevice());
		if (!support)
			throw std::runtime_error("Swapchain not supported!");

		m_Extent = support.capabilities.currentExtent;
		m_SwapchainFormat = pickSurfaceFormat(support.formats);

		vk::SwapchainCreateInfoKHR create_info;
		create_info.surface = m_Surface;

		create_info.imageFormat = m_SwapchainFormat.format;
		create_info.imageColorSpace = m_SwapchainFormat.colorSpace;
		create_info.imageExtent = m_Extent;

		create_info.imageArrayLayers = 1;
		create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

		if (support.capabilities.maxImageCount > 0
		&& support.capabilities.minImageCount + 1 > support.capabilities.maxImageCount)
			create_info.minImageCount = support.capabilities.maxImageCount;
		else
			create_info.minImageCount = support.capabilities.minImageCount + 1;

		create_info.imageSharingMode = vk::SharingMode::eExclusive;

		create_info.preTransform = support.capabilities.currentTransform;
		create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		create_info.presentMode = pickPresentMode(support.present_modes);
		create_info.clipped = true;

		m_Swapchain = logical_device.createSwapchainKHR(create_info);
		
		u32 img_count;
		(void)logical_device.getSwapchainImagesKHR(m_Swapchain, &img_count, nullptr);
		m_Images.resize(img_count);
		(void)logical_device.getSwapchainImagesKHR(m_Swapchain, &img_count, m_Images.ptr());
	}

	void Renderer::_create_image_views(void)
	{
		m_ImageViews.resize(m_Images.size());
		for (u64 i = 0; i < m_Images.size(); i++)
			m_ImageViews[i] = CreateImageView(
				m_Images[i],
				vk::ImageAspectFlagBits::eColor,
				m_SwapchainFormat.format
			);
	}

	void Renderer::_create_depth_buffer(void)
	{
		vk::Format depth_format = FindSupportedFormat(
			{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment
		);

		m_DepthImage = DeviceImage(
			{ m_Width, m_Height, 1 },
			vk::ImageAspectFlagBits::eDepth,
			depth_format,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eDepthStencilAttachment,
			vk::SharingMode::eExclusive,
			vk::SampleCountFlagBits::e1,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
		m_DepthImageView = CreateImageView(m_DepthImage.img, vk::ImageAspectFlagBits::eDepth, depth_format);
	}

	void Renderer::_create_render_pass(void)
	{
		std::array<vk::AttachmentDescription, 2> attachments{};
		attachments.fill({});

		vk::Format depth_format = FindSupportedFormat(
			{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment
		);

		vk::AttachmentDescription& color_attachment = attachments[0];
		vk::AttachmentDescription& depth_attachment = attachments[1];

		vk::AttachmentReference color_attachment_ref;
		vk::AttachmentReference depth_attachment_ref;

		vk::SubpassDescription subpass;
		vk::SubpassDependency dependency;

		color_attachment.format         = m_SwapchainFormat.format;
		color_attachment.samples        = vk::SampleCountFlagBits::e1;
		color_attachment.loadOp         = vk::AttachmentLoadOp::eClear;
		color_attachment.storeOp        = vk::AttachmentStoreOp::eStore;
		color_attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
		color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		color_attachment.initialLayout  = vk::ImageLayout::eUndefined;
		color_attachment.finalLayout    = vk::ImageLayout::ePresentSrcKHR;

		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout     = vk::ImageLayout::eColorAttachmentOptimal;

		depth_attachment.format         = depth_format;
		depth_attachment.samples        = vk::SampleCountFlagBits::e1;
		depth_attachment.loadOp         = vk::AttachmentLoadOp::eClear;
		depth_attachment.storeOp        = vk::AttachmentStoreOp::eDontCare;
		depth_attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
		depth_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depth_attachment.initialLayout  = vk::ImageLayout::eUndefined;
		depth_attachment.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		depth_attachment_ref.attachment = 1;
		depth_attachment_ref.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;

		subpass.colorAttachmentCount    = 1;
		subpass.pColorAttachments       = &color_attachment_ref;

		subpass.pDepthStencilAttachment = &depth_attachment_ref;

		dependency.srcSubpass           = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass           = 0;

		dependency.srcStageMask         = vk::PipelineStageFlagBits::eColorAttachmentOutput
			                            | vk::PipelineStageFlagBits::eLateFragmentTests;

		dependency.srcAccessMask        = vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		dependency.dstStageMask         = vk::PipelineStageFlagBits::eColorAttachmentOutput
			                            | vk::PipelineStageFlagBits::eEarlyFragmentTests;

		dependency.dstAccessMask        = vk::AccessFlagBits::eColorAttachmentWrite
			                            | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		vk::RenderPassCreateInfo create_info;

		create_info.attachmentCount = (u32)attachments.size();
		create_info.pAttachments = attachments.data();

		create_info.subpassCount = 1;
		create_info.pSubpasses = &subpass;

		create_info.dependencyCount = 1;
		create_info.pDependencies = &dependency;

		m_RenderPass = VkContext::GetLogicalDevice().createRenderPass(create_info);
	}

	void Renderer::_create_framebuffers(void)
	{
		m_Framebuffers.resize(m_ImageViews.size());
		for (u64 i = 0; i < m_ImageViews.size(); i++)
		{
			std::array<vk::ImageView, 2> attachments = {
				m_ImageViews[i],
				m_DepthImageView
			};

			vk::FramebufferCreateInfo create_info;

			create_info.renderPass = m_RenderPass;

			create_info.attachmentCount = (u32)attachments.size();
			create_info.pAttachments = attachments.data();

			create_info.width = m_Width;
			create_info.height = m_Height;
			create_info.layers = 1;

			m_Framebuffers[i] = VkContext::GetLogicalDevice().createFramebuffer(create_info);
		}
	}

	void Renderer::_create_command_objects(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		m_CurrentFrame = 0;

		vk::CommandPoolCreateInfo graphics_pool_info;
		graphics_pool_info.queueFamilyIndex = m_QueueIndices.graphics;
		graphics_pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

		m_GraphicsCmdPool = logical_device.createCommandPool(graphics_pool_info);

		vk::CommandBufferAllocateInfo cmd_alloc_info;
		cmd_alloc_info.commandPool = m_GraphicsCmdPool;
		cmd_alloc_info.level = vk::CommandBufferLevel::ePrimary;
		cmd_alloc_info.commandBufferCount = m_Config.max_frames_in_flight;

		m_GraphicsCmdBuffers.resize(m_Config.max_frames_in_flight);
		(void)logical_device.allocateCommandBuffers(&cmd_alloc_info, m_GraphicsCmdBuffers.ptr());

		vk::CommandPoolCreateInfo single_time_pool_info;
		single_time_pool_info.queueFamilyIndex = m_QueueIndices.graphics;
		single_time_pool_info.flags = vk::CommandPoolCreateFlagBits::eTransient;

		m_SingleTimeCmdPool = logical_device.createCommandPool(single_time_pool_info);
	}

	void Renderer::_create_sync_objects(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		m_Sync.image_available.resize(m_Config.max_frames_in_flight);
		m_Sync.render_finished.resize(m_Config.max_frames_in_flight);
		m_Sync.in_flight.resize(m_Config.max_frames_in_flight);

		vk::SemaphoreCreateInfo semaphore_info;

		vk::FenceCreateInfo fence_info;
		fence_info.flags = vk::FenceCreateFlagBits::eSignaled;

		for (u32 i = 0; i < m_Config.max_frames_in_flight; i++)
		{
			m_Sync.image_available[i] = logical_device.createSemaphore(semaphore_info);
			m_Sync.render_finished[i] = logical_device.createSemaphore(semaphore_info);
			m_Sync.in_flight[i] = logical_device.createFence(fence_info);
		}
	}

	void Renderer::_recreate_swapchain(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		m_Width = m_Window->width();
		m_Height = m_Window->height();

		m_Viewport.width = (float)m_Width;
		m_Viewport.height = (float)m_Height;
		m_Scissor.extent.width = m_Width;
		m_Scissor.extent.height = m_Height;

		logical_device.waitIdle();

		m_HasResized = false;

		for (auto& framebuffer : m_Framebuffers)
			logical_device.destroyFramebuffer(framebuffer);

		m_DepthImage.destroy();
		if (m_DepthImageView)
			logical_device.destroyImageView(m_DepthImageView);

		for (auto& img_view : m_ImageViews)
			logical_device.destroyImageView(img_view);

		logical_device.destroySwapchainKHR(m_Swapchain);

		_create_swapchain();
		_create_image_views();
		_create_depth_buffer();
		_create_framebuffers();
	}

	inline void Renderer::set_viewport(const glm::vec4& viewport)
	{
		m_Viewport.x = viewport.x;
		m_Viewport.y = viewport.y;
		m_Viewport.width = viewport.z;
		m_Viewport.height = viewport.w;

		m_Scissor.extent.width = (u32)ceil(viewport.z);
		m_Scissor.extent.height = (u32)ceil(viewport.w);
	}

	vk::CommandBuffer BeginSingleTimeCommands(vk::CommandPool cmd_pool)
	{
		vk::CommandBufferAllocateInfo alloc_info;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandPool = cmd_pool;
		alloc_info.commandBufferCount = 1;

		vk::CommandBuffer cmd_buffer;
		(void)VkContext::GetLogicalDevice().allocateCommandBuffers(&alloc_info, &cmd_buffer);

		vk::CommandBufferBeginInfo begin_info;
		begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		cmd_buffer.begin(begin_info);

		return cmd_buffer;
	}

	void EndSingleTimeCommands(vk::CommandBuffer cmd_buffer, vk::CommandPool cmd_pool)
	{
		cmd_buffer.end();

		vk::SubmitInfo submit_info;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &cmd_buffer;

		(void)VkContext::GetGraphicsQueue().submit(1, &submit_info, nullptr);
		VkContext::GetGraphicsQueue().waitIdle();

		VkContext::GetLogicalDevice().freeCommandBuffers(cmd_pool, 1, &cmd_buffer);
	}
} // namespace Na
