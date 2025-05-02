#include "Pch.hpp"
#include "Natrium/Graphics/Buffers/VertexBuffer.hpp"

#include "Natrium/Graphics/VkContext.hpp"

#include "Natrium/Core/Logger.hpp"

namespace Na {
	VertexBuffer::VertexBuffer(u64 size, const void* data, Renderer& renderer)
	: m_Buffer(
		size,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	)
	{
		this->set_data(data, renderer);
	}

	void VertexBuffer::destroy(void)
	{
		m_Buffer.destroy();
	}

	void VertexBuffer::set_data(const void* data, Renderer& renderer)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		DeviceBuffer stage_buffer(
			m_Buffer.size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		void* memory = logical_device.mapMemory(stage_buffer.memory, 0, m_Buffer.size);
		memcpy(memory, data, m_Buffer.size);
		logical_device.unmapMemory(stage_buffer.memory);

		m_Buffer.copy(stage_buffer, renderer.single_time_cmd_pool());

		stage_buffer.destroy();
	}

	VertexBuffer::VertexBuffer(VertexBuffer&& other)
	: m_Buffer(std::move(other.m_Buffer))
	{}

	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other)
	{
		this->destroy();
		m_Buffer = std::move(other.m_Buffer);
		return *this;
	}
} // namespace Na
