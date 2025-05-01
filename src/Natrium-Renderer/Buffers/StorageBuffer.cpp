#include "Pch.hpp"
#include "Natrium-Renderer/Buffers/StorageBuffer.hpp"

#include "Natrium-Renderer/VkContext.hpp"
#include "Natrium-Renderer/Pipeline.hpp"
#include "Internal.hpp"

namespace Na {
	StorageBuffer::StorageBuffer(u64 size, Renderer& renderer)
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

	void StorageBuffer::bind_to_pipeline(u32 binding, GraphicsPipeline& pipeline) const
	{
		for (u64 i = 0; i < m_BufferDatas.size(); i++)
		{
			vk::DescriptorBufferInfo buffer_info(m_BufferDatas[i].buffer, 0, m_Size);

			Internal::WriteToDescriptorSet(
				pipeline.descriptor_sets()[i],
				binding,
				vk::DescriptorType::eStorageBuffer,
				1, // count
				&buffer_info,
				nullptr, // image info
				nullptr // texel buffer view
			);
		}
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
