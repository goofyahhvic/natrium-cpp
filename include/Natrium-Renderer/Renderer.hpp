#if !defined(NA_RENDERER_HPP)
#define NA_RENDERER_HPP

#include "Natrium-Core/Window.hpp"
#include "./DeviceImage.hpp"
#include "./Colors.hpp"
#include "./Shader.hpp"

namespace Na {
	struct PushConstant {
		ShaderStageBits shader_stage;
		u32 size;
		u32 offset = 0;
	};

	struct RendererConfig {
		u32 max_frames_in_flight = 2;

		bool anisotropy_enabled = true;
		float max_anisotropy;

		bool msaa_enabled = true;

		RendererConfig(void);
	};

	struct Frame {
		bool              skipped = false;

		vk::CommandBuffer cmd_buffer;

		vk::Semaphore     image_available_semaphore;
		vk::Semaphore     render_finished_semaphore;
		vk::Fence         in_flight_fence;

		[[nodiscard]] inline operator bool(void) const { return !skipped; }
	};

	class Renderer {
	public:
		Renderer(void) = default;
		Renderer(Window& window, const RendererConfig& config = RendererConfig());
		void destroy(void);
		inline ~Renderer(void) { this->destroy(); }

		Renderer(const Renderer& other) = delete;
		Renderer& operator=(const Renderer& other) = delete;

		Renderer(Renderer&& other);
		Renderer& operator=(Renderer&& other);

		Frame& clear(const glm::vec4& color = Colors::k_Black);
		void present(void);

		void set_push_constant(PushConstant push_constant, void* data);

		[[nodiscard]] inline RendererConfig config(void) const { return m_Config; }
		[[nodiscard]] inline vk::CommandPool single_time_cmd_pool(void) const { return m_SingleTimeCmdPool; }

		[[nodiscard]] inline Frame& current_frame(void) { return m_Frames[m_CurrentFrame]; }
		[[nodiscard]] inline const Frame& current_frame(void) const { return m_Frames[m_CurrentFrame]; }
		[[nodiscard]] inline u32 current_frame_index(void) const { return m_CurrentFrame; }

		inline void bind_pipeline(u64 pipeline_handle) { m_PipelineHandle = pipeline_handle; }
		[[nodiscard]] inline u64 pipeline_handle(void) const { return m_PipelineHandle; }

		[[nodiscard]] inline u32 width(void) const { return m_Width; }
		[[nodiscard]] inline u32 height(void) const { return m_Height; }

		inline void set_viewport(const glm::vec4& viewport);
		[[nodiscard]] inline glm::vec4 viewport(void) const { return { m_Viewport.x, m_Viewport.y, m_Viewport.width, m_Viewport.height }; }

		[[nodiscard]] inline operator bool(void) const { return m_Window; }
	private:
		void _create_window_surface(void);
		void _create_swapchain(void);
		void _create_image_views(void);
		void _create_color_buffer(void);
		void _create_depth_buffer(void);
		void _create_render_pass(void);
		void _create_framebuffers(void);
		void _create_command_objects(void);
		void _create_sync_objects(void);

		void _recreate_swapchain(void);
	private:
		friend class Pipeline;

		Window* m_Window = nullptr;
		vk::SurfaceKHR m_Surface;

		union {
			struct { u32 m_Width, m_Height; };
			glm::uvec2 m_Size = { 0, 0 };
			vk::Extent2D m_Extent;
		};

		QueueFamilyIndices m_QueueIndices;

		vk::Viewport m_Viewport;
		vk::Rect2D m_Scissor;

		vk::SwapchainKHR m_Swapchain;
		vk::SurfaceFormatKHR m_SwapchainFormat;

		Na::ArrayVector<vk::Image> m_Images;
		Na::ArrayVector<vk::ImageView> m_ImageViews;

		DeviceImage m_ColorImage;
		vk::ImageView m_ColorImageView;

		DeviceImage m_DepthImage;
		vk::ImageView m_DepthImageView;

		vk::RenderPass m_RenderPass;
		ArrayVector<vk::Framebuffer> m_Framebuffers;

		vk::CommandPool m_GraphicsCmdPool;
		vk::CommandPool m_SingleTimeCmdPool;

		ArrayVector<Frame> m_Frames;
		u32 m_CurrentFrame = 0;
		u32 m_ImageIndex = 0;

		u64 m_PipelineHandle = NA_INVALID_HANDLE;

		RendererConfig m_Config{};
	};

	vk::CommandBuffer BeginSingleTimeCommands(vk::CommandPool cmd_pool);
	void EndSingleTimeCommands(vk::CommandBuffer cmd_buffer, vk::CommandPool cmd_pool);
} // namespace Na

#endif // NA_RENDERER_HPP