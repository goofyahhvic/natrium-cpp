#include "Pch.hpp"
#include "Natrium/Graphics/Buffers/UniformBuffer.hpp"

#include "Natrium/Graphics/VkContext.hpp"
#include "Natrium/Graphics/Pipeline.hpp"
#include "Internal.hpp"

namespace Na {
	UniformBuffer::UniformBuffer(u64 size, const RendererSettings& renderer_settings)
	: m_PerFrameSize(size)
	{
		static VkDeviceSize x_Alignment = VkContext::GetPhysicalDevice().getProperties().limits.minUniformBufferOffsetAlignment;

		m_AlignedSize = (size + x_Alignment - 1) & ~(x_Alignment - 1);

		m_Buffer = DeviceBuffer(
			m_AlignedSize * renderer_settings.max_frames_in_flight,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		m_Mapped = VkContext::GetLogicalDevice().mapMemory(m_Buffer.memory, 0, size);
	}

	void UniformBuffer::destroy(void)
	{
		m_Buffer.destroy();
	}

	void UniformBuffer::bind_to_pipeline(u32 binding, GraphicsPipeline& pipeline) const
	{
		vk::DescriptorBufferInfo buffer_info(m_Buffer.buffer, 0, m_AlignedSize);

		Internal::WriteToDescriptorSet(
			pipeline.descriptor_set(),
			binding,
			vk::DescriptorType::eUniformBuffer,
			1, // count
			&buffer_info,
			nullptr, // image info
			nullptr // texel buffer view
		);
		for (u64 i = pipeline.dynamic_offset_index(); i < pipeline.dynamic_offsets().size(); i += pipeline.dynamic_offset_count())
			pipeline.dynamic_offsets()[i] = u32(m_AlignedSize * i);
		pipeline.increment_dynamic_offset_index();
	}

	UniformBuffer::UniformBuffer(UniformBuffer&& other)
	: m_Buffer(std::move(other.m_Buffer)),
	m_Mapped(std::exchange(other.m_Mapped, nullptr)),
	m_PerFrameSize(other.m_PerFrameSize),
	m_AlignedSize(other.m_AlignedSize)
	{}

	UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other)
	{
		m_Buffer = std::move(other.m_Buffer);
		m_Mapped = std::exchange(other.m_Mapped, nullptr);
		m_PerFrameSize = other.m_PerFrameSize;
		m_AlignedSize = other.m_AlignedSize;

		return *this;
	}
} // namespace Na
