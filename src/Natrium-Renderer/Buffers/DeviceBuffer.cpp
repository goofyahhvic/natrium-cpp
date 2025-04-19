#include "Pch.hpp"
#include "Natrium-Renderer/Buffers/DeviceBuffer.hpp"

#include "Natrium-Renderer/VkContext.hpp"
#include "Natrium-Renderer/Renderer.hpp"

namespace Na {
	u32 FindMemoryType(u32 typeFilter, vk::MemoryPropertyFlags properties)
	{
		vk::PhysicalDeviceMemoryProperties memory_properties = VkContext::GetPhysicalDevice().getMemoryProperties();

		for (u32 i = 0; i < memory_properties.memoryTypeCount; i++)
			if ((typeFilter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;

		return 0;
	}

	DeviceBuffer::DeviceBuffer(
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::SharingMode sharing_mode
	)
	: size(size)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		vk::BufferCreateInfo buffer_info;
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = sharing_mode;

		this->buffer = logical_device.createBuffer(buffer_info);

		vk::MemoryRequirements memory_requirements = logical_device.getBufferMemoryRequirements(this->buffer);

		vk::MemoryAllocateInfo alloc_info;
		alloc_info.allocationSize = memory_requirements.size;
		alloc_info.memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, properties);

		this->memory = logical_device.allocateMemory(alloc_info);
		logical_device.bindBufferMemory(this->buffer, this->memory, 0);
	}

	void DeviceBuffer::destroy(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		logical_device.destroyBuffer(this->buffer);
		logical_device.freeMemory(this->memory);

		memset(this, 0, sizeof(DeviceBuffer));
	}

	void DeviceBuffer::copy(const DeviceBuffer& other, vk::CommandPool cmd_pool)
	{
		vk::CommandBuffer cmd_buffer = BeginSingleTimeCommands(cmd_pool);

		vk::BufferCopy copy_region;
		copy_region.srcOffset = 0;
		copy_region.dstOffset = 0;
		copy_region.size = other.size;

		cmd_buffer.copyBuffer(other.buffer, this->buffer, 1, &copy_region);

		EndSingleTimeCommands(cmd_buffer, cmd_pool);
	}

} // namespace Na
