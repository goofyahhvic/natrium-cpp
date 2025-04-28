#if !defined(NA_STORAGE_BUFFER_HPP)
#define NA_STORAGE_BUFFER_HPP

#include "./DeviceBuffer.hpp"
#include "../Renderer.hpp"

namespace Na {
	class StorageBuffer {
	public:
		StorageBuffer(void) = default;
		StorageBuffer(u64 size, u32 binding, Renderer& renderer);
		void destroy(void);
		inline ~StorageBuffer(void) { this->destroy(); }

		StorageBuffer(const StorageBuffer& other) = delete;
		StorageBuffer& operator=(const StorageBuffer& other) = delete;

		StorageBuffer(StorageBuffer&& other);
		StorageBuffer& operator=(StorageBuffer&& other);

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

#endif // NA_STORAGE_BUFFER_HPP