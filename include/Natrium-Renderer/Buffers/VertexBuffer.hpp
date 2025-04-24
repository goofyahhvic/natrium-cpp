#if !defined(NA_VERTEX_BUFFER_HPP)
#define NA_VERTEX_BUFFER_HPP

#include "./DeviceBuffer.hpp"
#include "../Renderer.hpp"

namespace Na {
	class VertexBuffer {
	public:
		VertexBuffer(void) = default;
		VertexBuffer(u32 count, u32 vertex_size, void* data, Renderer& renderer);
		void destroy(void);
		inline ~VertexBuffer(void) { this->destroy(); }

		VertexBuffer(const VertexBuffer& other) = delete;
		VertexBuffer& operator=(const VertexBuffer& other) = delete;

		VertexBuffer(VertexBuffer&& other);
		VertexBuffer& operator=(VertexBuffer&& other);

		void set_data(void* data, Renderer& renderer);

		void draw(Renderer& renderer) const;

		[[nodiscard]] inline u64 size(void) const { return m_Buffer.size; }
		[[nodiscard]] inline u32 count(void) const { return m_Count; }
		[[nodiscard]] inline const DeviceBuffer& buffer(void) const { return m_Buffer; }

		[[nodiscard]] inline operator bool(void) const { return m_Count; }
	private:
		DeviceBuffer m_Buffer;
		u32 m_Count = 0;
	};
} // namespace Na

#endif // NA_VERTEX_BUFFER_HPP