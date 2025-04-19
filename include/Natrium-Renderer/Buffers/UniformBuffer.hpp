#if !defined(NA_UNIFORM_BUFFER_HPP)
#define NA_UNIFORM_BUFFER_HPP

#include "./DeviceBuffer.hpp"
#include "../Renderer.hpp"

namespace Na {
	class UniformBuffer {
	public:
		UniformBuffer(u64 size, u32 binding, Renderer& renderer);
		void destroy(void);

		void set_data(void* data, Renderer& renderer);

		[[nodiscard]] inline u64 size(void) const { return m_Size; }
		[[nodiscard]] inline operator bool(void) const { return !m_Buffers.empty(); }
	private:
		Na::ArrayVector<vk::Buffer> m_Buffers;
		Na::ArrayVector<vk::DeviceMemory> m_Memories;
		Na::ArrayVector<void*> m_Mapped;

		u64 m_Size;
	};
} // namespace Na

#endif // NA_UNIFORM_BUFFER_HPP