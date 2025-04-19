#if !defined(NA_INDEX_BUFFER_HPP)
#define NA_INDEX_BUFFER_HPP

#include "./VertexBuffer.hpp"

namespace Na {
	class IndexBuffer {
	public:
		IndexBuffer(u32* data, u32 count, Renderer& renderer);
		void destroy(void);

		void set_data(u32* data, Renderer& renderer);

		void draw(const VertexBuffer& vbo, Renderer& renderer) const;

		[[nodiscard]] inline u64 size(void) const { return m_Buffer.size; }
		[[nodiscard]] inline u32 count(void) const { return m_Count; }

		[[nodiscard]] inline operator bool(void) const { return m_Buffer; }
	private:
		DeviceBuffer m_Buffer;
		u32 m_Count;
	};
} // namespace Na

#endif // NA_INDEX_BUFFER_HPP