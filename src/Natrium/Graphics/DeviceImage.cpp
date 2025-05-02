#include "Pch.hpp"
#include "Natrium/Graphics/DeviceImage.hpp"

#include "Natrium/Graphics/VkContext.hpp"
#include "Natrium/Graphics/Buffers/DeviceBuffer.hpp"
#include "Natrium/Graphics/Renderer.hpp"

namespace Na {
	vk::Format FindSupportedFormat(
		const std::initializer_list<vk::Format>& candidates,
		vk::ImageTiling tiling,
		vk::FormatFeatureFlags features
	)
	{
		for (vk::Format format : candidates)
		{
			vk::FormatProperties properties = VkContext::GetPhysicalDevice().getFormatProperties(format);

			if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
				return format;
			else
			if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
				return format;
		}

		return vk::Format::eUndefined;
	}

	DeviceImage::DeviceImage(
		const vk::Extent3D& extent,
		vk::ImageAspectFlagBits aspect_mask,
		vk::Format format,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::SharingMode sharing_mode,
		vk::SampleCountFlagBits sample_count,
		vk::MemoryPropertyFlags memory_properties
	)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		this->extent = extent;
		this->format = format;

		this->subresource_range.aspectMask = aspect_mask;

		this->subresource_range.baseMipLevel = 0;
		this->subresource_range.levelCount = 1;

		this->subresource_range.baseArrayLayer = 0;
		this->subresource_range.layerCount = 1;

		
		vk::ImageCreateInfo create_info;

		create_info.imageType = vk::ImageType::e2D;

		create_info.extent = extent;
		create_info.mipLevels = 1;
		create_info.arrayLayers = 1;

		create_info.format = format;

		create_info.tiling = vk::ImageTiling::eOptimal;
		create_info.initialLayout = vk::ImageLayout::eUndefined;

		create_info.usage = usage;
		create_info.sharingMode = sharing_mode;

		create_info.samples = sample_count;

		this->img = logical_device.createImage(create_info);

		vk::MemoryRequirements memory_requirements = logical_device.getImageMemoryRequirements(this->img);

		vk::MemoryAllocateInfo alloc_info;
		alloc_info.allocationSize = memory_requirements.size;
		alloc_info.memoryTypeIndex = FindMemoryType(
			memory_requirements.memoryTypeBits,
			memory_properties
		);

		this->memory = logical_device.allocateMemory(alloc_info);
		logical_device.bindImageMemory(this->img, this->memory, 0);
	}

	void DeviceImage::destroy(void)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		if (this->img)
			logical_device.destroyImage(this->img);

		if (this->memory)
			logical_device.freeMemory(this->memory);

		memset(this, 0, sizeof(DeviceImage));
	}

	void DeviceImage::transition_layout(
		vk::ImageLayout old_layout,
		vk::ImageLayout new_layout,
		vk::CommandPool cmd_pool
	)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		vk::CommandBuffer cmd_buffer = BeginSingleTimeCommands(cmd_pool);

		vk::ImageMemoryBarrier barrier;

		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = this->img;
		barrier.subresourceRange = this->subresource_range;

		vk::PipelineStageFlags execute_stage;
		vk::PipelineStageFlags wait_stage;

		if (old_layout == vk::ImageLayout::eUndefined
		 && new_layout == vk::ImageLayout::eTransferDstOptimal)
		{
			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
			
			execute_stage = vk::PipelineStageFlagBits::eTopOfPipe;
			wait_stage = vk::PipelineStageFlagBits::eTransfer;
		} else
		if (old_layout == vk::ImageLayout::eTransferDstOptimal
		 && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			execute_stage = vk::PipelineStageFlagBits::eTransfer;
			wait_stage = vk::PipelineStageFlagBits::eFragmentShader;
		} else
		{
			throw std::runtime_error("Unsupported image layout transition!");
		}

		cmd_buffer.pipelineBarrier(
			execute_stage,
			wait_stage,
			{}, // dependency flags
			0, nullptr, // memory barriers
			0, nullptr, // buffer memory barriers
			1, &barrier // image memory barriers
		);

		EndSingleTimeCommands(cmd_buffer, cmd_pool);
	}

	void DeviceImage::copy_from_buffer(vk::Buffer buffer, vk::CommandPool cmd_pool)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		vk::CommandBuffer cmd_buffer = BeginSingleTimeCommands(cmd_pool);

		vk::BufferImageCopy region;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = this->subresource_range.aspectMask;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = {{ 0, 0, 0 }};
		region.imageExtent = this->extent;

		cmd_buffer.copyBufferToImage(
			buffer,
			this->img,
			vk::ImageLayout::eTransferDstOptimal,
			1, &region
		);

		EndSingleTimeCommands(cmd_buffer, cmd_pool);
	}

	DeviceImage::DeviceImage(DeviceImage&& other)
	: img(std::exchange(other.img, nullptr)),
	memory(std::exchange(other.memory, nullptr)),
	extent(std::exchange(other.extent, { 0, 0, 0 })),
	format(std::exchange(other.format, vk::Format::eUndefined)),
	subresource_range(std::move(other.subresource_range))
	{}

	DeviceImage& DeviceImage::operator=(DeviceImage&& other)
	{
		this->destroy();
		this->img = std::exchange(other.img, nullptr);
		this->memory = std::exchange(other.memory, nullptr);
		this->extent = std::exchange(other.extent, { 0, 0, 0 });
		this->format = std::exchange(other.format, vk::Format::eUndefined);
		this->subresource_range = std::move(other.subresource_range);
		return *this;
	}

	vk::ImageView CreateImageView(
		vk::Image img,
		vk::ImageAspectFlagBits aspect_mask,
		vk::Format format
	)
	{
		vk::ImageViewCreateInfo create_info;

		create_info.image = img;
		create_info.viewType = vk::ImageViewType::e2D;

		create_info.format = format;

		create_info.subresourceRange.aspectMask = aspect_mask;

		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;

		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		return VkContext::GetLogicalDevice().createImageView(create_info);
	}
} // namespace Na
