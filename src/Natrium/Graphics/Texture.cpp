#include "Pch.hpp"
#include "Natrium/Graphics/Texture.hpp"

#include "Natrium/Graphics/Buffers/DeviceBuffer.hpp"
#include "Natrium/Graphics/VkContext.hpp"
#include "Natrium/Graphics/Pipeline.hpp"
#include "Internal.hpp"

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

	Texture::Texture(const Image& img, const RendererSettings& renderer_settings)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		DeviceBuffer buffer(
			img.size(),
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		void* data = logical_device.mapMemory(buffer.memory, 0, img.size());
		memcpy(data, img.data(), img.size());
		logical_device.unmapMemory(buffer.memory);

		m_Image = DeviceImage(
			{ (u32)img.width(), (u32)img.height(), 1 }, // extent
			vk::ImageAspectFlagBits::eColor,
			vk::Format::eR8G8B8A8Srgb,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
			vk::SharingMode::eExclusive,
			vk::SampleCountFlagBits::e1,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		m_Image.transition_layout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		m_Image.copy_from_buffer(buffer.buffer);
		m_Image.transition_layout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

		buffer.destroy();

		m_ImageView = CreateImageView(m_Image.img, vk::ImageAspectFlagBits::eColor, vk::Format::eR8G8B8A8Srgb);

		m_Sampler = createSampler(
			vk::Filter::eLinear, // oversampling filter
			vk::Filter::eLinear, // undersampling filter
			renderer_settings.anisotropy_enabled,
			renderer_settings.max_anisotropy
		);
	}

	void Texture::destroy(void)
	{
		if (!m_Image)
			return;

		vk::Device logical_device = VkContext::GetLogicalDevice();

		logical_device.destroySampler(m_Sampler);
		m_Sampler = nullptr;

		logical_device.destroyImageView(m_ImageView);
		m_ImageView = nullptr;

		m_Image.destroy();
	}

	Texture::Texture(Texture&& other)
	: m_Image(std::move(other.m_Image)),
	m_ImageView(std::exchange(other.m_ImageView, nullptr)),
	m_Sampler(std::exchange(other.m_Sampler, nullptr))
	{}

	Texture& Texture::operator=(Texture&& other)
	{
		this->destroy();
		m_Image = std::move(other.m_Image);
		m_ImageView = std::exchange(other.m_ImageView, nullptr);
		m_Sampler = std::exchange(other.m_Sampler, nullptr);
		return *this;
	}
} // namespace Na
