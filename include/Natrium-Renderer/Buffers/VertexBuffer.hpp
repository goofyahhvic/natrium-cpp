#if !defined(NA_VERTEX_BUFFER_HPP)
#define NA_VERTEX_BUFFER_HPP

#include "./DeviceBuffer.hpp"
#include "../Renderer.hpp"

namespace Na {
	class VertexBuffer {
	public:
		VertexBuffer(void* data, u64 size, u32 count, Renderer& renderer);
		void destroy(void);

		void set_data(void* data, Renderer& renderer);

		void draw(Renderer& renderer) const;

		[[nodiscard]] inline u64 size(void) const { return m_Buffer.size; }
		[[nodiscard]] inline u32 count(void) const { return m_Count; }
		[[nodiscard]] inline const DeviceBuffer& buffer(void) const { return m_Buffer; }

		[[nodiscard]] inline operator bool(void) const { return m_Buffer; }
	private:
		DeviceBuffer m_Buffer;
		u32 m_Count;
	};
} // namespace Na

#endif // NA_VERTEX_BUFFER_HPP