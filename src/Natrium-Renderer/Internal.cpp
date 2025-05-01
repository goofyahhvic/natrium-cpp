#include "Pch.hpp"
#include "Internal.hpp"

#include "Natrium-Renderer/VkContext.hpp"

namespace Na {
	void Internal::WriteToDescriptorSet(
		vk::DescriptorSet set,
		u32 binding,
		vk::DescriptorType type,
		u32 count,
		vk::DescriptorBufferInfo* buffer_info, 
		vk::DescriptorImageInfo* image_info,
		vk::BufferView* texel_buffer_view
	)
	{
		vk::WriteDescriptorSet descriptor_write;
		descriptor_write.dstSet = set;
		descriptor_write.dstBinding = binding;
		descriptor_write.dstArrayElement = 0;

		descriptor_write.descriptorType = type;
		descriptor_write.descriptorCount = 1;

		descriptor_write.pBufferInfo = buffer_info;
		descriptor_write.pImageInfo = image_info;
		descriptor_write.pTexelBufferView = texel_buffer_view;

		VkContext::GetLogicalDevice().updateDescriptorSets(
			1, &descriptor_write,
			0, nullptr // descriptor copy
		);
	}
} // namespace Na
