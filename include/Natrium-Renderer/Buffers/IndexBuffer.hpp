#if !defined(NA_INDEX_BUFFER_HPP)
#define NA_INDEX_BUFFER_HPP

#include "./VertexBuffer.hpp"

namespace Na {
	class IndexBuffer {
	public:
		IndexBuffer(void) = default;
		IndexBuffer(u32 count, const u32* data, Renderer& renderer);
		void destroy(void);

		IndexBuffer(const IndexBuffer& other) = delete;
		IndexBuffer& operator=(const IndexBuffer& other) = delete;

		IndexBuffer(IndexBuffer&& other);
		IndexBuffer& operator=(IndexBuffer&& other);

		void set_data(const u32* data, Renderer& renderer);

		void draw(const VertexBuffer& vertex_buffer, u32 instance_count, Renderer& renderer) const;
		inline void draw(const VertexBuffer& vertex_buffer, Renderer& renderer) const { this->draw(vertex_buffer, 1, renderer); }

		[[nodiscard]] inline u64 size(void) const { return m_Buffer.size; }
		[[nodiscard]] inline u32 count(void) const { return m_Count; }

		[[nodiscard]] inline operator bool(void) const { return m_Count; }
		[[nodiscard]] inline vk::Buffer native(void) const { return m_Buffer.buffer; }
	private:
		DeviceBuffer m_Buffer;
		u32 m_Count = 0;
	};
} // namespace Na

#endif // NA_INDEX_BUFFER_HPP