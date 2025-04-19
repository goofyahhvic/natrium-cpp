#include "Pch.hpp"
#include "Natrium-Renderer/Texture.hpp"

#include "Natrium-Renderer/Buffers/DeviceBuffer.hpp"
#include "Natrium-Renderer/VkContext.hpp"

namespace Na {
	static vk::Sampler createSampler(
		vk::Filter oversampling_filter,
		vk::Filter undersampling_filter,
		bool anisotropy_enabled,
		float max_anisotropy
	)
	{
		vk::SamplerCreateInfo create_info;

		create_info.magFilter = oversampling_filter;
		create_info.minFilter = undersampling_filter;

		create_info.addressModeU = vk::SamplerAddressMode::eRepeat;
		create_info.addressModeV = vk::SamplerAddressMode::eRepeat;
		create_info.addressModeW = vk::SamplerAddressMode::eRepeat;

		create_info.anisotropyEnable = anisotropy_enabled;
		create_info.maxAnisotropy = max_anisotropy;

		create_info.borderColor = vk::BorderColor::eIntOpaqueBlack;

		create_info.unnormalizedCoordinates = VK_FALSE;

		create_info.compareEnable = VK_FALSE;
		create_info.compareOp = vk::CompareOp::eAlways;

		create_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		create_info.mipLodBias = 0.0f;
		create_info.minLod = 0.0f;
		create_info.maxLod = 0.0f;

		return VkContext::GetLogicalDevice().createSampler(create_info);
	}

	Texture::Texture(const Image& img, u32 binding, Renderer& renderer)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		DeviceBuffer buffer(
			img.size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		void* data = logical_device.mapMemory(buffer.memory, 0, img.size);
		memcpy(data, (void*)img.data, img.size);
		logical_device.unmapMemory(buffer.memory);

		m_Image = DeviceImage(
			{ (u32)img.width, (u32)img.height, 1, }, // extent
			vk::ImageAspectFlagBits::eColor,
			vk::Format::eR8G8B8A8Srgb,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
			vk::SharingMode::eExclusive,
			vk::SampleCountFlagBits::e1,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		m_Image.transition_layout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, renderer.single_time_cmd_pool());
		m_Image.copy_from_buffer(buffer.buffer, renderer.single_time_cmd_pool());
		m_Image.transition_layout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, renderer.single_time_cmd_pool());

		buffer.destroy();

		m_ImageView = CreateImageView(m_Image.img, vk::ImageAspectFlagBits::eColor, vk::Format::eR8G8B8A8Srgb);

		m_Sampler = createSampler(
			vk::Filter::eLinear, // oversampling filter
			vk::Filter::eLinear, // undersampling filter
			renderer.config().anisotropy_enabled,
			renderer.config().max_anisotropy
		);

		vk::DescriptorImageInfo descriptor_info;
		descriptor_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		descriptor_info.imageView = m_ImageView;
		descriptor_info.sampler = m_Sampler;

		for (size_t i = 0; i < renderer.config().max_frames_in_flight; i++)
		{
			vk::WriteDescriptorSet descriptor_write;

			descriptor_write.dstSet = VkContext::GetPipelinePool()[renderer.pipeline_handle()].descriptor_sets[i];
			descriptor_write.dstBinding = binding;
			descriptor_write.dstArrayElement = 0;
			descriptor_write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptor_write.descriptorCount = 1;
			descriptor_write.pImageInfo = &descriptor_info;

			logical_device.updateDescriptorSets(
				1, &descriptor_write,
				0, nullptr // descriptor copy
			);
		}
	}

	void Texture::destroy(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		if (m_Sampler)
			logical_device.destroySampler(m_Sampler);
		m_Sampler = nullptr;

		if (m_ImageView)
			logical_device.destroyImageView(m_ImageView);
		m_ImageView = nullptr;

		m_Image.destroy();
	}
} // namespace Na
