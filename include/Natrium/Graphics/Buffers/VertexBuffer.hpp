#if !defined(NA_VERTEX_BUFFER_HPP)
#define NA_VERTEX_BUFFER_HPP

#include "Natrium/Graphics/Buffers/DeviceBuffer.hpp"
#include "Natrium/Graphics/Renderer.hpp"

namespace Na {
	class VertexBuffer {
	public:
		VertexBuffer(void) = default;
		VertexBuffer(u64 size, const void* data, Renderer& renderer);
		void destroy(void);
		inline ~VertexBuffer(void) { this->destroy(); }

		VertexBuffer(const VertexBuffer& other) = delete;
		VertexBuffer& operator=(const VertexBuffer& other) = delete;

		VertexBuffer(VertexBuffer&& other);
		VertexBuffer& operator=(VertexBuffer&& other);

		void set_data(const void* data, Renderer& renderer);

		[[nodiscard]] inline u64 size(void) const { return m_Buffer.size; }
		[[nodiscard]] inline const DeviceBuffer& buffer(void) const { return m_Buffer; }

		[[nodiscard]] inline operator bool(void) const { return m_Buffer; }
		[[nodiscard]] inline vk::Buffer native(void) const { return m_Buffer.buffer; }
	private:
		DeviceBuffer m_Buffer;
	};
} // namespace Na

#endif // NA_VERTEX_BUFFER_HPP