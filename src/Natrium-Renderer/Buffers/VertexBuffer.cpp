#include "Pch.hpp"
#include "Natrium-Renderer/Buffers/VertexBuffer.hpp"

#include "Natrium-Renderer/VkContext.hpp"

namespace Na {
	VertexBuffer::VertexBuffer(void* data, u64 size, u32 count, Renderer& renderer)
	: m_Count(count),
	m_Buffer(
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
		if (!frame)
			return;

		vk::Buffer vertex_buffers[] = { m_Buffer.buffer };
		vk::DeviceSize offsets[] = { 0 };

		frame.cmd_buffer.bindVertexBuffers(0, 1, vertex_buffers, offsets);
		frame.cmd_buffer.draw(m_Count, 1, 0, 0);
	}
} // namespace Na
