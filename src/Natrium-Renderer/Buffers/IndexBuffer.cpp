#include "Pch.hpp"
#include "Natrium-Renderer/Buffers/IndexBuffer.hpp"

#include "Natrium-Renderer/VkContext.hpp"

namespace Na {
	IndexBuffer::IndexBuffer(u32 count, u32* data, Renderer& renderer)
	: m_Count(count),
	m_Buffer(
		count * sizeof(u32),
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	)
	{
		this->set_data(data, renderer);
	}

	void IndexBuffer::destroy(void)
	{
		m_Buffer.destroy();
	}

	void IndexBuffer::set_data(u32* data, Renderer& renderer)
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

	void IndexBuffer::draw(const VertexBuffer& vertex_buffer, u32 instance_count, Renderer& renderer) const
	{
		FrameData& fd = renderer.current_frame();

		fd.cmd_buffer.bindVertexBuffers(0, { vertex_buffer.native() }, {0});
		fd.cmd_buffer.bindIndexBuffer(m_Buffer.buffer, 0, vk::IndexType::eUint32);
		fd.cmd_buffer.drawIndexed(m_Count, instance_count, 0, 0, 0);
	}

	IndexBuffer::IndexBuffer(IndexBuffer&& other)
	: m_Buffer(std::move(other.m_Buffer)),
	m_Count(std::exchange(other.m_Count, 0))
	{}

	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other)
	{
		m_Buffer = std::move(other.m_Buffer);
		m_Count = std::exchange(other.m_Count, 0);
		return *this;
	}
} // namespace Na
