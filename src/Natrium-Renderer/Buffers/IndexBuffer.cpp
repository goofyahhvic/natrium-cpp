#include "Pch.hpp"
#include "Natrium-Renderer/Buffers/IndexBuffer.hpp"

#include "Natrium-Renderer/VkContext.hpp"

namespace Na {
	IndexBuffer::IndexBuffer(u32* data, u32 count, Renderer& renderer)
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

	void IndexBuffer::draw(const VertexBuffer& vertex_buffer, Renderer& renderer) const
	{
		Frame& frame = renderer.current_frame();

		vk::DeviceSize offsets[] = { 0 };

		frame.cmd_buffer.bindVertexBuffers(0, 1, &vertex_buffer.buffer().buffer, offsets);
		frame.cmd_buffer.bindIndexBuffer(m_Buffer.buffer, 0, vk::IndexType::eUint32);
		frame.cmd_buffer.drawIndexed(m_Count, 1, 0, 0, 0);
	}

	/*
	void IndexBuffer::draw(const std::initializer_list<VertexBuffer>& vertex_buffers, Renderer& renderer) const
	{
		Frame& frame = renderer.current_frame();

		Na::ArrayVector<vk::Buffer> buffers(vertex_buffers.size());
		for (u64 i = 0; const VertexBuffer& buffer : vertex_buffers)
			buffers[i++] = buffer.buffer().buffer;
		vk::DeviceSize offsets[] = { 0, 24 * 12 };

		frame.cmd_buffer.bindVertexBuffers(0, 2, buffers.ptr(), offsets);
		frame.cmd_buffer.bindIndexBuffer(m_Buffer.buffer, 0, vk::IndexType::eUint32);
		frame.cmd_buffer.drawIndexed(m_Count, 1, 0, 0, 0);
	}
	*/
} // namespace Na
