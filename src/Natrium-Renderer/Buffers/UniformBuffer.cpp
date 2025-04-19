#include "Pch.hpp"
#include "Natrium-Renderer/Buffers/UniformBuffer.hpp"

#include "Natrium-Renderer/VkContext.hpp"

namespace Na {
	UniformBuffer::UniformBuffer(u64 size, u32 binding, Renderer& renderer)
	: m_Size(size)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		u32 max_frames_in_flight = renderer.config().max_frames_in_flight;

		m_Buffers.resize(max_frames_in_flight);
		m_Memories.resize(max_frames_in_flight);
		m_Mapped.resize(max_frames_in_flight);

		for (u64 i = 0; i < max_frames_in_flight; i++)
		{
			DeviceBuffer buffer(
				size,
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			m_Buffers[i] = buffer.buffer;
			m_Memories[i] = buffer.memory;

			m_Mapped[i] = logical_device.mapMemory(m_Memories[i], 0, size);

			vk::DescriptorBufferInfo buffer_info;
			buffer_info.buffer = m_Buffers[i];
			buffer_info.offset = 0;
			buffer_info.range = (u32)size;

			vk::WriteDescriptorSet descriptor_write;
			descriptor_write.dstSet = VkContext::GetPipelinePool()[renderer.pipeline_handle()].descriptor_sets[i];
			descriptor_write.dstBinding = binding;
			descriptor_write.dstArrayElement = 0;

			descriptor_write.descriptorType = vk::DescriptorType::eUniformBuffer;
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

	void UniformBuffer::destroy(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		for (u64 i = 0; i < m_Buffers.size(); i++)
		{
			logical_device.destroyBuffer(m_Buffers[i]);
			logical_device.freeMemory(m_Memories[i]);
		}

		m_Buffers.clear();
		m_Memories.clear();
		m_Mapped.clear();

		m_Size = 0;
	}

	void UniformBuffer::set_data(void* data, Renderer& renderer)
	{
		memcpy(m_Mapped[renderer.current_frame()], data, m_Size);
	}

} // namespace Na
