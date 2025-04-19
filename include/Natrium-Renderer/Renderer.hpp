#if !defined(NA_RENDERER_HPP)
#define NA_RENDERER_HPP

#include "Natrium-Core/Window.hpp"
#include "./DeviceImage.hpp"
#include "./Colors.hpp"

namespace Na {
	struct RendererConfig {
		u32 max_frames_in_flight;
		bool anisotropy_enabled;
		float max_anisotropy;
	};

	class Renderer {
	public:
		Renderer(Window& window);
		void destroy(void);

		void clear(const glm::vec4& color = Colors::k_Black);
		void present(void);

		void update_size(void);

		[[nodiscard]] inline RendererConfig config(void) const { return m_Config; }
		[[nodiscard]] inline vk::CommandPool single_time_cmd_pool(void) const { return m_SingleTimeCmdPool; }

		[[nodiscard]] inline vk::CommandBuffer current_graphics_cmd_buffer(void) const { return m_GraphicsCmdBuffers[m_CurrentFrame]; }

		inline void bind_pipeline(u64 pipeline_handle) { m_PipelineHandle = pipeline_handle; }
		[[nodiscard]] inline u64 pipeline_handle(void) const { return m_PipelineHandle; }

		[[nodiscard]] inline u32 width(void) const { return m_Width; }
		[[nodiscard]] inline u32 height(void) const { return m_Height; }

		inline void set_viewport(const glm::vec4& viewport);
		[[nodiscard]] inline glm::vec4 viewport(void) const { return { m_Viewport.x, m_Viewport.y, m_Viewport.width, m_Viewport.height }; }

		[[nodiscard]] inline u32 current_frame(void) const { return m_CurrentFrame; }
	private:
		void _create_window_surface(void);
		void _create_swapchain(void);
		void _create_image_views(void);
		void _create_depth_buffer(void);
		void _create_render_pass(void);
		void _create_framebuffers(void);
		void _create_command_objects(void);
		void _create_sync_objects(void);

		void _recreate_swapchain(void);
	private:
		friend class Pipeline;

		Window* m_Window;
		vk::SurfaceKHR m_Surface;

		union {
			struct { u32 m_Width, m_Height; };
			glm::uvec2 m_Size;
			vk::Extent2D m_Extent;
		};

		bool m_HasResized;

		QueueFamilyIndices m_QueueIndices;

		vk::Viewport m_Viewport;
		vk::Rect2D m_Scissor;

		vk::SwapchainKHR m_Swapchain;
		vk::SurfaceFormatKHR m_SwapchainFormat;

		Na::ArrayVector<vk::Image> m_Images;
		Na::ArrayVector<vk::ImageView> m_ImageViews;

		DeviceImage m_DepthImage;
		vk::ImageView m_DepthImageView;

		vk::RenderPass m_RenderPass;
		Na::ArrayVector<vk::Framebuffer> m_Framebuffers;

		vk::CommandPool m_GraphicsCmdPool;
		Na::ArrayVector<vk::CommandBuffer> m_GraphicsCmdBuffers;

		vk::CommandPool m_SingleTimeCmdPool;

		struct Sync {
			Na::ArrayVector<vk::Semaphore> image_available;
			Na::ArrayVector<vk::Semaphore> render_finished;
			Na::ArrayVector<vk::Fence>     in_flight;
		};
		Sync m_Sync;

		u32 m_CurrentFrame;
		u32 m_ImageIndex;

		u64 m_PipelineHandle;

		RendererConfig m_Config;
	};

	vk::CommandBuffer BeginSingleTimeCommands(vk::CommandPool cmd_pool);
	void EndSingleTimeCommands(vk::CommandBuffer cmd_buffer, vk::CommandPool cmd_pool);
} // namespace Na

#endif // NA_RENDERER_HPP