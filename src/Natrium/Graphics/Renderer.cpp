#include "Pch.hpp"
#include "Natrium/Graphics/Renderer.hpp"

#include "Natrium/Core/Logger.hpp"

#include "Natrium/Graphics/VkContext.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Na {
	extern vk::SurfaceKHR createWindowSurface(GLFWwindow* window);

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

	RendererConfig::RendererConfig(void)
	{
		max_anisotropy = VkContext::GetPhysicalDevice().getProperties().limits.maxSamplerAnisotropy;
	}

	void FrameData::bind_pipeline(const GraphicsPipeline& pipeline)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		this->cmd_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline());

		if (!pipeline.descriptor_sets().empty())
			this->cmd_buffer.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				pipeline.layout(),
				0, // first set
				1, &pipeline.descriptor_sets()[this->index],
				0, nullptr // dynamic offsets
			);
	}

	void FrameData::set_push_constant(
		PushConstant push_constant,
		const void* data,
		const GraphicsPipeline& pipeline
	)
	{
		this->cmd_buffer.pushConstants(
			pipeline.layout(),
			(vk::ShaderStageFlagBits)push_constant.shader_stage,
			push_constant.offset,
			push_constant.size,
			data
		);
	}

	Renderer::Renderer(Window& window, const RendererConfig& config)
	: m_Window(&window),
	m_Config(config)
	{
		m_Frames.resize(m_Config.max_frames_in_flight);
		for (u32 i = 0; i < (u32)m_Frames.size(); i++)
		{
			m_Frames[i].index = i;
			m_Frames[i].renderer = this;
		}

		_create_window_surface();
		_create_swapchain();
		_create_image_views();
		_create_color_buffer();
		_create_depth_buffer();
		_create_render_pass();
		_create_framebuffers();
		_create_command_objects();
		_create_sync_objects();
	}

	void Renderer::destroy(void)
	{
		if (!m_Window)
			return;

		vk::Device logical_device = VkContext::GetLogicalDevice();

		for (FrameData& frame : m_Frames)
		{
			logical_device.destroyFence(frame.in_flight_fence);
			logical_device.destroySemaphore(frame.render_finished_semaphore);
			logical_device.destroySemaphore(frame.image_available_semaphore);
		}

		logical_device.destroyCommandPool(m_SingleTimeCmdPool);
		logical_device.destroyCommandPool(m_GraphicsCmdPool);

		for (vk::Framebuffer framebuffer : m_Framebuffers)
			logical_device.destroyFramebuffer(framebuffer);

		logical_device.destroyRenderPass(m_RenderPass);

		m_DepthImage.destroy();
		logical_device.destroyImageView(m_DepthImageView);

		m_ColorImage.destroy();
		logical_device.destroyImageView(m_ColorImageView);

		for (auto& img_view : m_ImageViews)
			logical_device.destroyImageView(img_view);

		logical_device.destroySwapchainKHR(m_Swapchain);

		VkContext::GetInstance().destroySurfaceKHR(m_Surface);

		m_Window = nullptr;
	}

	FrameData& Renderer::begin_frame(const glm::vec4& color)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();
		FrameData& fd = m_Frames[m_CurrentFrame];

		fd.skipped = false;

		if (m_Width != m_Window->width() ||
			m_Height != m_Window->height())
		{
			_recreate_swapchain();
			fd.skipped = true;
			return fd;
		}

		vk::Result result = vk::Result::eSuccess;
		
		result = logical_device.waitForFences(
			1, &fd.in_flight_fence,
			VK_TRUE, // wait all
			UINT64_MAX // timeout
		);
		NA_CHECK_VK(result, "Failed to wait for in flight fence!");

		try {
			result = logical_device.acquireNextImageKHR(
				m_Swapchain,
				UINT64_MAX, // timeout
				fd.image_available_semaphore,
				nullptr,
				&m_ImageIndex
			);

			if (result == vk::Result::eErrorOutOfDateKHR)
			{
				_recreate_swapchain();
				fd.skipped = true;
				return fd;
			} else
			if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
				throw std::runtime_error(NA_FORMAT("Result {}: Failed to acquire swapchain image!", (int)result));
		} catch (const vk::OutOfDateKHRError& err)
		{
			(void)err;
			_recreate_swapchain();
			fd.skipped = true;
			return fd;
		}

		result = logical_device.resetFences(1, &fd.in_flight_fence);
		NA_CHECK_VK(result, "Failed to acquire reset in flight fence!");
		fd.cmd_buffer.reset();

		vk::CommandBufferBeginInfo begin_info;
		fd.cmd_buffer.begin(begin_info);

		std::array<vk::ClearValue, 2> clear_values;
		clear_values[0].color = std::array<float, 4>{ color.r, color.g, color.g, color.a };
		clear_values[1].depthStencil = {{ 1.0f, 0 }};

		vk::RenderPassBeginInfo render_pass_info;

		render_pass_info.renderPass = m_RenderPass;
		render_pass_info.framebuffer = m_Framebuffers[m_ImageIndex];

		render_pass_info.renderArea.offset = {{ 0, 0 }};
		render_pass_info.renderArea.extent = m_Extent;

		render_pass_info.clearValueCount = (u32)clear_values.size();
		render_pass_info.pClearValues = clear_values.data();

		fd.cmd_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);

		fd.cmd_buffer.setViewport(0, 1, &m_Viewport);
		fd.cmd_buffer.setScissor(0, 1, &m_Scissor);

		return fd;
	}

	void Renderer::end_frame(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();
		FrameData& fd = m_Frames[m_CurrentFrame];

		vk::Result result = vk::Result::eSuccess;

		fd.cmd_buffer.endRenderPass();
		fd.cmd_buffer.end();

		vk::SubmitInfo submit_info;

		vk::Semaphore wait_semaphores[] = { fd.image_available_semaphore };
		vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

		submit_info.waitSemaphoreCount = sizeof(wait_semaphores) / sizeof(vk::Semaphore);
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;

		vk::Semaphore signal_semaphores[] = { fd.render_finished_semaphore };
		submit_info.signalSemaphoreCount = sizeof(signal_semaphores) / sizeof(VkSemaphore);
		submit_info.pSignalSemaphores = signal_semaphores;

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &fd.cmd_buffer;

		result = VkContext::GetGraphicsQueue().submit(1, &submit_info, fd.in_flight_fence);
		NA_CHECK_VK(result, "Failed to submit draw calls to graphics queue!");

		vk::PresentInfoKHR present_info;
		present_info.waitSemaphoreCount = submit_info.waitSemaphoreCount;
		present_info.pWaitSemaphores = signal_semaphores;

		present_info.swapchainCount = 1;
		present_info.pSwapchains = &m_Swapchain;
		present_info.pImageIndices = &m_ImageIndex;

		try {
			result = VkContext::GetGraphicsQueue().presentKHR(present_info);
			switch(result) {
				case vk::Result::eSuboptimalKHR:
					_recreate_swapchain();
					break;
				case vk::Result::eErrorOutOfDateKHR:
					_recreate_swapchain();
					break;
				case vk::Result::eSuccess:
					break;
				default:
					NA_CHECK_VK(result, "Failed to present to graphics queue!");
			}

		} catch (const vk::OutOfDateKHRError& err)
		{
			(void)err;
			_recreate_swapchain();
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % m_Config.max_frames_in_flight;
	}

	void Renderer::_create_window_surface(void)
	{
		m_Surface = createWindowSurface(m_Window->native());
		m_QueueIndices = QueueFamilyIndices::Get(VkContext::GetPhysicalDevice(), m_Surface);
		if (!m_QueueIndices)
			throw std::runtime_error("Queue indices is incomplete!");

		m_Width = m_Window->width();
		m_Height = m_Window->height();

		m_Viewport.x = 0.0f;
		m_Viewport.y = (float)m_Height;
		m_Viewport.width = (float)m_Width;
		m_Viewport.height = -(float)m_Height;
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

	void Renderer::_create_color_buffer(void)
	{
		m_ColorImage = DeviceImage(
			{ m_Width, m_Height, 1 },
			vk::ImageAspectFlagBits::eColor,
			m_SwapchainFormat.format,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive,
			VkContext::GetMSAASamples(m_Config.msaa_enabled),
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
		m_ColorImageView = CreateImageView(m_ColorImage.img, vk::ImageAspectFlagBits::eColor, m_SwapchainFormat.format);
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
			VkContext::GetMSAASamples(m_Config.msaa_enabled),
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
		m_DepthImageView = CreateImageView(m_DepthImage.img, vk::ImageAspectFlagBits::eDepth, depth_format);
	}

	void Renderer::_create_render_pass(void)
	{
		std::array<vk::AttachmentDescription, 3> attachments{};
		attachments.fill({});

		vk::Format depth_format = FindSupportedFormat(
			{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment
		);

		vk::AttachmentDescription& color_attachment = attachments[0];
		vk::AttachmentDescription& depth_attachment = attachments[1];
		vk::AttachmentDescription& color_attachment_resolve = attachments[2];

		vk::AttachmentReference color_attachment_ref;
		vk::AttachmentReference depth_attachment_ref;
		vk::AttachmentReference color_attachment_resolve_ref;

		vk::SubpassDescription subpass;
		vk::SubpassDependency dependency;

		color_attachment.format         = m_SwapchainFormat.format;
		color_attachment.samples        = VkContext::GetMSAASamples(m_Config.msaa_enabled);
		color_attachment.loadOp         = vk::AttachmentLoadOp::eClear;
		color_attachment.storeOp        = vk::AttachmentStoreOp::eStore;
		color_attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
		color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		color_attachment.initialLayout  = vk::ImageLayout::eUndefined;
		color_attachment.finalLayout    = vk::ImageLayout::eColorAttachmentOptimal;

		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout     = vk::ImageLayout::eColorAttachmentOptimal;

		depth_attachment.format         = depth_format;
		depth_attachment.samples        = VkContext::GetMSAASamples(m_Config.msaa_enabled);
		depth_attachment.loadOp         = vk::AttachmentLoadOp::eClear;
		depth_attachment.storeOp        = vk::AttachmentStoreOp::eDontCare;
		depth_attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
		depth_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depth_attachment.initialLayout  = vk::ImageLayout::eUndefined;
		depth_attachment.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		depth_attachment_ref.attachment = 1;
		depth_attachment_ref.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		color_attachment_resolve.format         = m_SwapchainFormat.format;
		color_attachment_resolve.samples        = vk::SampleCountFlagBits::e1;
		color_attachment_resolve.loadOp         = vk::AttachmentLoadOp::eDontCare;
		color_attachment_resolve.storeOp        = vk::AttachmentStoreOp::eStore;
		color_attachment_resolve.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
		color_attachment_resolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		color_attachment_resolve.initialLayout  = vk::ImageLayout::eUndefined;
		color_attachment_resolve.finalLayout    = vk::ImageLayout::ePresentSrcKHR;
			
		color_attachment_resolve_ref.attachment = 2;
		color_attachment_resolve_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;;

		subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;

		subpass.colorAttachmentCount    = 1;
		subpass.pColorAttachments       = &color_attachment_ref;
		subpass.pDepthStencilAttachment = &depth_attachment_ref;
		subpass.pResolveAttachments     = &color_attachment_resolve_ref;

		dependency.srcSubpass           = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass           = 0;

		dependency.srcStageMask         = vk::PipelineStageFlagBits::eColorAttachmentOutput
			                            | vk::PipelineStageFlagBits::eLateFragmentTests;

		dependency.srcAccessMask        = vk::AccessFlagBits::eColorAttachmentWrite
			                            | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

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
			std::array<vk::ImageView, 3> attachments = {
				m_ColorImageView,
				m_DepthImageView,
				m_ImageViews[i],
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

		vk::CommandPoolCreateInfo graphics_pool_info;
		graphics_pool_info.queueFamilyIndex = m_QueueIndices.graphics;
		graphics_pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

		m_GraphicsCmdPool = logical_device.createCommandPool(graphics_pool_info);

		vk::CommandBufferAllocateInfo cmd_alloc_info;
		cmd_alloc_info.commandPool = m_GraphicsCmdPool;
		cmd_alloc_info.level = vk::CommandBufferLevel::ePrimary;
		cmd_alloc_info.commandBufferCount = m_Config.max_frames_in_flight;

		for (u64 i = 0; auto cmd_buffer : logical_device.allocateCommandBuffers(cmd_alloc_info))
			m_Frames[i++].cmd_buffer = cmd_buffer;

		vk::CommandPoolCreateInfo single_time_pool_info;
		single_time_pool_info.queueFamilyIndex = m_QueueIndices.graphics;
		single_time_pool_info.flags = vk::CommandPoolCreateFlagBits::eTransient;

		m_SingleTimeCmdPool = logical_device.createCommandPool(single_time_pool_info);
	}

	void Renderer::_create_sync_objects(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		vk::SemaphoreCreateInfo semaphore_info;

		vk::FenceCreateInfo fence_info;
		fence_info.flags = vk::FenceCreateFlagBits::eSignaled;

		for (u32 i = 0; i < m_Config.max_frames_in_flight; i++)
		{
			m_Frames[i].image_available_semaphore = logical_device.createSemaphore(semaphore_info);
			m_Frames[i].render_finished_semaphore = logical_device.createSemaphore(semaphore_info);
			m_Frames[i].in_flight_fence = logical_device.createFence(fence_info);
		}
	}

	void Renderer::_recreate_swapchain(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		m_Width = m_Window->width();
		m_Height = m_Window->height();

		m_Viewport.y = (float)m_Height;
		m_Viewport.width = (float)m_Width;
		m_Viewport.height = -(float)m_Height;
		m_Scissor.extent.width = m_Width;
		m_Scissor.extent.height = m_Height;

		logical_device.waitIdle();

		for (auto& framebuffer : m_Framebuffers)
			logical_device.destroyFramebuffer(framebuffer);

		m_DepthImage.destroy();
		logical_device.destroyImageView(m_DepthImageView);

		m_ColorImage.destroy();
		logical_device.destroyImageView(m_ColorImageView);

		for (auto& img_view : m_ImageViews)
			logical_device.destroyImageView(img_view);

		logical_device.destroySwapchainKHR(m_Swapchain);

		_create_swapchain();
		_create_image_views();
		_create_color_buffer();
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

	Renderer::Renderer(Renderer&& other)
	: m_Window(std::exchange(other.m_Window, nullptr)),
	m_Surface(std::exchange(other.m_Surface, nullptr)),

	m_Width(other.m_Width),
	m_Height(other.m_Height),

	m_QueueIndices(std::move(other.m_QueueIndices)),

	m_Viewport(std::move(other.m_Viewport)),
	m_Scissor(std::move(other.m_Scissor)),

	m_Swapchain(std::exchange(other.m_Swapchain, nullptr)),
	m_SwapchainFormat(std::move(other.m_SwapchainFormat)),

	m_Images(std::move(other.m_Images)),
	m_ImageViews(std::move(other.m_ImageViews)),

	m_DepthImage(std::move(other.m_DepthImage)),
	m_DepthImageView(std::exchange(other.m_DepthImageView, nullptr)),

	m_ColorImage(std::move(other.m_ColorImage)),
	m_ColorImageView(std::exchange(other.m_ColorImageView, nullptr)),

	m_RenderPass(std::exchange(other.m_RenderPass, nullptr)),

	m_Framebuffers(std::move(other.m_Framebuffers)),

	m_GraphicsCmdPool(std::exchange(other.m_GraphicsCmdPool, nullptr)),
	m_SingleTimeCmdPool(std::exchange(other.m_SingleTimeCmdPool, nullptr)),

	m_Frames(std::move(other.m_Frames)),

	m_CurrentFrame(other.m_CurrentFrame),
	m_ImageIndex(other.m_ImageIndex),

	m_Config(std::move(other.m_Config))
	{}

	Renderer& Renderer::operator=(Renderer&& other)
	{
		this->destroy();

		m_Window = std::exchange(other.m_Window, nullptr);
		m_Surface = std::exchange(other.m_Surface, nullptr);

		m_Width = other.m_Width;
		m_Height = other.m_Height;

		m_QueueIndices = std::move(other.m_QueueIndices);

		m_Viewport = std::move(other.m_Viewport);
		m_Scissor = std::move(other.m_Scissor);

		m_Swapchain = std::exchange(other.m_Swapchain, nullptr);
		m_SwapchainFormat = std::move(other.m_SwapchainFormat);

		m_Images = std::move(other.m_Images);
		m_ImageViews = std::move(other.m_ImageViews);

		m_DepthImage = std::move(other.m_DepthImage);
		m_DepthImageView = std::exchange(other.m_DepthImageView, nullptr);

		m_ColorImage = std::move(other.m_ColorImage);
		m_ColorImageView = std::exchange(other.m_ColorImageView, nullptr);

		m_RenderPass = std::exchange(other.m_RenderPass, nullptr);

		m_Framebuffers = std::move(other.m_Framebuffers);

		m_GraphicsCmdPool = std::exchange(other.m_GraphicsCmdPool, nullptr);
		m_SingleTimeCmdPool = std::exchange(other.m_SingleTimeCmdPool, nullptr);

		m_Frames = std::move(other.m_Frames);

		m_CurrentFrame = other.m_CurrentFrame;
		m_ImageIndex = other.m_ImageIndex;

		m_Config = std::move(other.m_Config);

		return *this;
	}
} // namespace Na
