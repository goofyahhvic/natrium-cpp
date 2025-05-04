#if !defined(NA_RENDERER_HPP)
#define NA_RENDERER_HPP

#include "Natrium/Graphics/Renderer/RendererCore.hpp"
#include "Natrium/Graphics/Pipeline.hpp"

#include "Natrium/Graphics/Buffers/VertexBuffer.hpp"
#include "Natrium/Graphics/Buffers/IndexBuffer.hpp"
#include "Natrium/Graphics/Buffers/UniformBuffer.hpp"
#include "Natrium/Graphics/Buffers/StorageBuffer.hpp"

namespace Na {
	struct FrameData {
		bool              valid = false;

		vk::CommandBuffer cmd_buffer;

		vk::Semaphore     image_available_semaphore;
		vk::Semaphore     render_finished_semaphore;
		vk::Fence         in_flight_fence;
	};

	class Renderer {
	public:
		Renderer(void) = default;
		Renderer(RendererCore& renderer_core);

		void destroy(void);
		inline ~Renderer(void) { this->destroy(); }

		[[nodiscard]] bool begin_frame(const glm::vec4& color = Colors::k_Black);
		void end_frame(void);

		void bind_pipeline(const GraphicsPipeline& pipeline);
		void set_push_constant(const PushConstant& push_constant, const void* data, const GraphicsPipeline& pipeline);

		void draw_vertices(const VertexBuffer& vertex_buffer, u32 vertex_count, u32 instance_count = 1);
		void draw_indexed(const VertexBuffer& vertex_buffer, const IndexBuffer& index_buffer, u32 instance_count = 1);

		void set_uniform_buffer(UniformBuffer& uniform_buffer, const void* data) const;
		void set_storage_buffer(StorageBuffer& storage_buffer, const void* data) const;

		[[nodiscard]] inline RendererCore& core(void) { return *m_Core; }
		[[nodiscard]] inline const RendererCore& core(void) const { return *m_Core; }

		[[nodiscard]] inline FrameData& current_frame(void) { return m_Frames[m_CurrentFrame]; }
		[[nodiscard]] inline const FrameData& current_frame(void) const { return m_Frames[m_CurrentFrame]; }

		[[nodiscard]] inline u32 current_frame_index(void) const { return m_CurrentFrame; }

		[[nodiscard]] inline operator bool(void) const { return m_Core; }

		Renderer(const Renderer& other) = delete;
		Renderer& operator=(const Renderer& other) = delete;

		Renderer(Renderer&& other);
		Renderer& operator=(Renderer&& other);
	private:
		void _create_command_objects(void);
		void _create_sync_objects(void);
	private:
		RendererCore* m_Core = nullptr;

		vk::CommandPool m_GraphicsCmdPool;

		ArrayVector<FrameData> m_Frames;
		u32 m_CurrentFrame = 0;
		u32 m_ImageIndex = 0;
	};
} // namespace Na

#endif // NA_RENDERER_HPP