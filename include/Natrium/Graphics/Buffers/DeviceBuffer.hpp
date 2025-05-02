#if !defined(NA_DEVICE_BUFFER)
#define NA_DEVICE_BUFFER

#include "Natrium/Core.hpp"

namespace Na {
	u32 FindMemoryType(u32 typeFilter, vk::MemoryPropertyFlags properties);
	class DeviceBuffer {
	public:
		vk::Buffer buffer = nullptr;
		vk::DeviceSize size = 0;
		vk::DeviceMemory memory = nullptr;

		DeviceBuffer(void) = default;
		DeviceBuffer(
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			vk::MemoryPropertyFlags properties,
			vk::SharingMode sharing_mode = vk::SharingMode::eExclusive
		);
		void destroy(void);
		inline ~DeviceBuffer(void) { this->destroy(); }

		DeviceBuffer(const DeviceBuffer& other) = delete;
		DeviceBuffer& operator=(const DeviceBuffer& other) = delete;

		DeviceBuffer(DeviceBuffer&& other);
		DeviceBuffer& operator=(DeviceBuffer&& other);

		void copy(const DeviceBuffer& other, vk::CommandPool cmd_pool);

		[[nodiscard]] inline operator bool(void) const { return buffer && size && memory; }
	};
} // namespace Na

#endif // NA_DEVICE_BUFFER