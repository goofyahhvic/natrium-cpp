#include "Pch.hpp"
#include "Natrium/Graphics/Texture.hpp"

#include "Natrium/Graphics/Buffers/DeviceBuffer.hpp"
#include "Natrium/Graphics/VkContext.hpp"
#include "Natrium/Graphics/Pipeline.hpp"
#include "Internal.hpp"

namespace Na {
	Texture::Texture(
		const AssetHandle<Image>* imgs,
		u32 count,
		const RendererSettings& renderer_settings
	)
	{
		NA_ASSERT(imgs, "Failed to create TextureArray: imgs is null!");
		NA_ASSERT(count, "Failed to create TextureArray: count is 0!");
		NA_ASSERT(count <= VkContext::GetPhysicalDevice().getProperties().limits.maxImageArrayLayers,
				  "Failed to create TextureArray: image count exceeded gpu limit!");

		const AssetHandle<Image>& first_img = imgs[0];

		if (k_BuildConfig != BuildConfig::Distribution)
		{
			for (u32 i = 0; i < count; i++)
			{
				if (!*imgs[i])
					throw std::runtime_error(NA_FORMAT("Failed to create TextureArray: Invalid image at index {}", i));

				if (imgs[i]->size() != first_img->size())
					throw std::runtime_error("Failed to create TextureArray: Image at index {} has a different size!");
			}
		}

		vk::Device logical_device = VkContext::GetLogicalDevice();

		DeviceBuffer buffer(
			first_img->size() * count,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		void* data = logical_device.mapMemory(buffer.memory, 0, first_img->size() * count);
		for (u32 i = 0; i < count; i++)
			memcpy((Byte*)data + i * first_img->size(), imgs[i]->data(), imgs[i]->size());
		logical_device.unmapMemory(buffer.memory);

		m_Image = DeviceImage(
			{ (u32)first_img->width(), (u32)first_img->height(), 1 }, // extent
			count, // layer count
			vk::ImageAspectFlagBits::eColor,
			vk::Format::eR8G8B8A8Srgb,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
			vk::SharingMode::eExclusive,
			vk::SampleCountFlagBits::e1,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		m_Image.transition_layout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		m_Image.copy_all_from_buffer(buffer.buffer);
		m_Image.transition_layout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

		buffer.destroy();

		m_ImageView = m_Image.create_img_view();

		m_Sampler = Internal::CreateSampler(
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
