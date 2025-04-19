#if !defined(NA_DEVICE_BUFFER)
#define NA_DEVICE_BUFFER

#include "Natrium-Core/Core.hpp"

namespace Na {
	u32 FindMemoryType(u32 typeFilter, vk::MemoryPropertyFlags properties);
	class DeviceBuffer {
	public:
		vk::Buffer buffer;
		vk::DeviceSize size;
		vk::DeviceMemory memory;

		DeviceBuffer(void) : size(0) {}
		DeviceBuffer(
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			vk::MemoryPropertyFlags properties,
			vk::SharingMode sharing_mode = vk::SharingMode::eExclusive
		);
		void destroy(void);

		void copy(const DeviceBuffer& other, vk::CommandPool cmd_pool);

		[[nodiscard]] inline operator bool(void) const { return buffer && size && memory; }
	};
} // namespace Na

#endif // NA_DEVICE_BUFFER