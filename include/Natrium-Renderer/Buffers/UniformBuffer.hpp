#if !defined(NA_UNIFORM_BUFFER_HPP)
#define NA_UNIFORM_BUFFER_HPP

#include "./DeviceBuffer.hpp"
#include "../Renderer.hpp"

namespace Na {
	class UniformBuffer {
	public:
		UniformBuffer(void) = default;
		UniformBuffer(u64 size, u32 binding, Renderer& renderer);
		void destroy(void);
		inline ~UniformBuffer(void) { this->destroy(); }

		UniformBuffer(const UniformBuffer& other) = delete;
		UniformBuffer& operator=(const UniformBuffer& other) = delete;

		UniformBuffer(UniformBuffer&& other);
		UniformBuffer& operator=(UniformBuffer&& other);

		void set_data(void* data, Renderer& renderer);

		[[nodiscard]] inline u64 size(void) const { return m_Size; }
		[[nodiscard]] inline operator bool(void) const { return m_Size; }
	private:
		struct BufferData {
			vk::Buffer buffer;
			vk::DeviceMemory memory;
			void* mapped;
		};
		Na::ArrayVector<BufferData> m_BufferDatas;

		u64 m_Size = 0;
	};
} // namespace Na

#endif // NA_UNIFORM_BUFFER_HPP