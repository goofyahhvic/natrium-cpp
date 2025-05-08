#include "Pch.hpp"
#include "Natrium/Graphics/Renderer/RendererCore.hpp"

#include "Natrium/Core/Logger.hpp"

#include "Natrium/Graphics/VkContext.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

#if defined(NA_PLATFORM_WINDOWS) || defined(NA_PLATFORM_LINUX)

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#endif // NA_PLATFORM

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

	RendererCore::RendererCore(Window& window, const RendererSettings& settings)
	: m_Window(&window),
	m_Settings(settings)
	{
		_create_window_surface();
		_create_swapchain();
		_create_image_views();
		_create_color_buffer();
		_create_depth_buffer();
		_create_render_pass();
		_create_framebuffers();
	}

	void RendererCore::destroy(void)
	{
		if (!m_Window)
			return;

		vk::Device logical_device = VkContext::GetLogicalDevice();

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

	void RendererCore::_create_window_surface(void)
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

	void RendererCore::_create_swapchain(void)
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

	void RendererCore::_create_image_views(void)
	{
		m_ImageViews.resize(m_Images.size());
		for (u64 i = 0; i < m_Images.size(); i++)
			m_ImageViews[i] = CreateImageView(
				m_Images[i],
				vk::ImageAspectFlagBits::eColor,
				m_SwapchainFormat.format,
				1 // layer count
			);
	}

	void RendererCore::_create_color_buffer(void)
	{
		m_ColorImage = DeviceImage(
			{ m_Width, m_Height, 1 },
			1, // layer count
			vk::ImageAspectFlagBits::eColor,
			m_SwapchainFormat.format,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive,
			VkContext::GetMSAASamples(m_Settings.msaa_enabled),
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
		m_ColorImageView = CreateImageView(
			m_ColorImage.img,
			vk::ImageAspectFlagBits::eColor,
			m_SwapchainFormat.format,
			1 // layer count
		);
	}

	void RendererCore::_create_depth_buffer(void)
	{
		vk::Format depth_format = FindSupportedFormat(
			{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment
		);

		m_DepthImage = DeviceImage(
			{ m_Width, m_Height, 1 },
			1, // layer count
			vk::ImageAspectFlagBits::eDepth,
			depth_format,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eDepthStencilAttachment,
			vk::SharingMode::eExclusive,
			VkContext::GetMSAASamples(m_Settings.msaa_enabled),
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
		m_DepthImageView = CreateImageView(
			m_DepthImage.img,
			vk::ImageAspectFlagBits::eDepth,
			depth_format,
			1 // layer count
		);
	}

	void RendererCore::_create_render_pass(void)
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
		color_attachment.samples        = VkContext::GetMSAASamples(m_Settings.msaa_enabled);
		color_attachment.loadOp         = vk::AttachmentLoadOp::eClear;
		color_attachment.storeOp        = vk::AttachmentStoreOp::eStore;
		color_attachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
		color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		color_attachment.initialLayout  = vk::ImageLayout::eUndefined;
		color_attachment.finalLayout    = vk::ImageLayout::eColorAttachmentOptimal;

		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout     = vk::ImageLayout::eColorAttachmentOptimal;

		depth_attachment.format         = depth_format;
		depth_attachment.samples        = VkContext::GetMSAASamples(m_Settings.msaa_enabled);
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

	void RendererCore::_create_framebuffers(void)
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

	void RendererCore::_recreate_swapchain(void)
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

	inline void RendererCore::set_viewport(const glm::vec4& viewport)
	{
		m_Viewport.x = viewport.x;
		m_Viewport.y = viewport.y;
		m_Viewport.width = viewport.z;
		m_Viewport.height = viewport.w;

		m_Scissor.extent.width = (u32)ceil(viewport.z);
		m_Scissor.extent.height = (u32)ceil(viewport.w);
	}

	RendererCore::RendererCore(RendererCore&& other)
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

	m_Settings(other.m_Settings)
	{}

	RendererCore& RendererCore::operator=(RendererCore&& other)
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

		m_Settings = std::move(other.m_Settings);

		return *this;
	}
} // namespace Na
