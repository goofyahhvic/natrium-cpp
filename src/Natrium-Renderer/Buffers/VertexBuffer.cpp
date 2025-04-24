#include "Pch.hpp"
#include "Natrium-Renderer/Buffers/VertexBuffer.hpp"

#include "Natrium-Renderer/VkContext.hpp"

#include "Natrium-Core/Logger.hpp"

namespace Na {
	VertexBuffer::VertexBuffer(u32 count, u32 vertex_size, void* data, Renderer& renderer)
	: m_Count(count),
	m_Buffer(
		(u64)vertex_size * count,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	)
	{
		this->set_data(data, renderer);
	}

	void VertexBuffer::destroy(void)
	{
		if (!m_Count)
			return;

		m_Buffer.destroy();
	}

	void VertexBuffer::set_data(void* data, Renderer& renderer)
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

	void VertexBuffer::draw(Renderer& renderer) const
	{
		Frame& frame = renderer.current_frame();

		vk::DeviceSize offsets[] = { 0 };
		frame.cmd_buffer.bindVertexBuffers(0, 1, &m_Buffer.buffer, offsets);
		frame.cmd_buffer.draw(m_Count, 1, 0, 0);
	}

	VertexBuffer::VertexBuffer(VertexBuffer&& other)
	: m_Buffer(std::move(other.m_Buffer)),
	m_Count(std::exchange(other.m_Count, 0))
	{}

	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other)
	{
		this->destroy();
		m_Buffer = std::move(other.m_Buffer);
		m_Count = std::exchange(other.m_Count, 0);
		return *this;
	}
} // namespace Na
