#include "Pch.hpp"
#include "Natrium-Renderer/Buffers/StorageBuffer.hpp"

#include "Natrium-Renderer/VkContext.hpp"

namespace Na {
	StorageBuffer::StorageBuffer(u64 size, u32 binding, Renderer& renderer)
	: m_Size(size)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		u32 max_frames_in_flight = renderer.config().max_frames_in_flight;

		m_BufferDatas.resize(max_frames_in_flight);

		for (u64 i = 0; i < max_frames_in_flight; i++)
		{
			DeviceBuffer buffer(
				size,
				vk::BufferUsageFlagBits::eStorageBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			m_BufferDatas[i].buffer = std::exchange(buffer.buffer, nullptr);
			m_BufferDatas[i].memory = std::exchange(buffer.memory, nullptr);
			m_BufferDatas[i].mapped = logical_device.mapMemory(m_BufferDatas[i].memory, 0, size);

			vk::DescriptorBufferInfo buffer_info;
			buffer_info.buffer = m_BufferDatas[i].buffer;
			buffer_info.offset = 0;
			buffer_info.range = (u32)size;

			vk::WriteDescriptorSet descriptor_write;
			descriptor_write.dstSet = VkContext::GetPipelinePool()[renderer.pipeline_handle()].descriptor_sets[i];
			descriptor_write.dstBinding = binding;
			descriptor_write.dstArrayElement = 0;

			descriptor_write.descriptorType = vk::DescriptorType::eStorageBuffer;
			descriptor_write.descriptorCount = 1;

			descriptor_write.pBufferInfo = &buffer_info;
			descriptor_write.pImageInfo = nullptr;
			descriptor_write.pTexelBufferView = nullptr;

			logical_device.updateDescriptorSets(
				1, &descriptor_write,
				0, nullptr // descriptor copy
			);
		}
	}

	void StorageBuffer::destroy(void)
	{
		if (!m_Size)
			return;

		vk::Device logical_device = VkContext::GetLogicalDevice();

		for (const BufferData& buffer_data : m_BufferDatas)
		{
			logical_device.destroyBuffer(buffer_data.buffer);
			logical_device.freeMemory(buffer_data.memory);
		}

		m_BufferDatas.clear();
		m_Size = 0;
	}

	void StorageBuffer::set_data(void* data, Renderer& renderer)
	{
		memcpy(m_BufferDatas[renderer.current_frame_index()].mapped, data, m_Size);
	}

	StorageBuffer::StorageBuffer(StorageBuffer&& other)
	: m_BufferDatas(std::move(other.m_BufferDatas)),
	m_Size(std::exchange(other.m_Size, 0))
	{}

	StorageBuffer& StorageBuffer::operator=(StorageBuffer&& other)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		for (const BufferData& buffer_data : m_BufferDatas)
		{
			logical_device.destroyBuffer(buffer_data.buffer);
			logical_device.freeMemory(buffer_data.memory);
		}

		m_BufferDatas = std::move(other.m_BufferDatas);
		m_Size = std::exchange(other.m_Size, 0);

		return *this;
	}
} // namespace Na
