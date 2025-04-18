#if !defined(NA_DEVICE_BUFFER)
#define NA_DEVICE_BUFFER

#include "../VkContext.hpp"

namespace Na {
	u32 FindMemoryType(u32 typeFilter, vk::MemoryPropertyFlags properties);
	class DeviceBuffer {
	public:
		vk::Buffer buffer;
		vk::DeviceSize size;
		vk::DeviceMemory memory;

		DeviceBuffer(
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			vk::MemoryPropertyFlags properties,
			vk::SharingMode sharing_mode = vk::SharingMode::eExclusive
		);

		void copy(const DeviceBuffer& other, vk::CommandPool cmd_pool);
	};
} // namespace Na

#endif // NA_DEVICE_BUFFER