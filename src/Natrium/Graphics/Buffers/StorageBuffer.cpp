#include "Pch.hpp"
#include "Natrium/Graphics/Buffers/StorageBuffer.hpp"

#include "Natrium/Graphics/VkContext.hpp"
#include "Natrium/Graphics/Pipeline.hpp"
#include "Internal.hpp"

namespace Na {
	StorageBuffer::StorageBuffer(u64 size, const RendererSettings& renderer_settings)
	: m_Size(size)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		u32 max_frames_in_flight = renderer_settings.max_frames_in_flight;

		m_Datas.resize(max_frames_in_flight);

		for (u64 i = 0; i < max_frames_in_flight; i++)
		{
			DeviceBuffer buffer(
				size,
				vk::BufferUsageFlagBits::eStorageBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			m_Datas[i].buffer = std::exchange(buffer.buffer, nullptr);
			m_Datas[i].memory = std::exchange(buffer.memory, nullptr);
			m_Datas[i].mapped = logical_device.mapMemory(m_Datas[i].memory, 0, size);
		}
	}

	void StorageBuffer::destroy(void)
	{
		if (!m_Size)
			return;

		vk::Device logical_device = VkContext::GetLogicalDevice();

		for (const Data& buffer_data : m_Datas)
		{
			logical_device.destroyBuffer(buffer_data.buffer);
			logical_device.freeMemory(buffer_data.memory);
		}

		m_Datas.clear();
		m_Size = 0;
	}

	void StorageBuffer::bind_to_pipeline(u32 binding, GraphicsPipeline& pipeline) const
	{
		for (u64 i = 0; i < m_Datas.size(); i++)
		{
			vk::DescriptorBufferInfo buffer_info(m_Datas[i].buffer, 0, m_Size);

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

	StorageBuffer::StorageBuffer(StorageBuffer&& other)
	: m_Datas(std::move(other.m_Datas)),
	m_Size(std::exchange(other.m_Size, 0))
	{}

	StorageBuffer& StorageBuffer::operator=(StorageBuffer&& other)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		for (const Data& buffer_data : m_Datas)
		{
			logical_device.destroyBuffer(buffer_data.buffer);
			logical_device.freeMemory(buffer_data.memory);
		}

		m_Datas = std::move(other.m_Datas);
		m_Size = std::exchange(other.m_Size, 0);

		return *this;
	}
} // namespace Na
