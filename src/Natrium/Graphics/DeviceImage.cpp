#include "Pch.hpp"
#include "Natrium/Graphics/DeviceImage.hpp"

#include "Natrium/Graphics/VkContext.hpp"
#include "Natrium/Graphics/Buffers/DeviceBuffer.hpp"

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
		u32 layer_count,
		vk::ImageAspectFlagBits aspect_mask,
		vk::Format format,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::SharingMode sharing_mode,
		vk::SampleCountFlagBits sample_count,
		vk::MemoryPropertyFlags memory_properties
	)
	: extent(extent), format(format), layer_count(layer_count)
	{
		NA_ASSERT(layer_count > 0, "Failed to create DeviceImage: Invalid layer count!");

		vk::Device logical_device = VkContext::GetLogicalDevice();

		this->subresource_range.aspectMask = aspect_mask;

		this->subresource_range.baseMipLevel = 0;
		this->subresource_range.levelCount = 1;

		this->subresource_range.baseArrayLayer = 0;
		this->subresource_range.layerCount = 1;
		
		vk::ImageCreateInfo create_info;

		if (extent.depth == 1)
			create_info.imageType = vk::ImageType::e2D;
		else if (extent.depth > 1)
			create_info.imageType = vk::ImageType::e3D;
		else 
			throw std::runtime_error("Failed to create DeviceImage: Invalid depth!");

		create_info.extent = extent;
		create_info.mipLevels = 1;
		create_info.arrayLayers = layer_count;

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

	void DeviceImage::transition_layout(vk::ImageLayout old_layout, vk::ImageLayout new_layout)
	{
		vk::Device logical_device = VkContext::GetLogicalDevice();

		vk::CommandBuffer cmd_buffer = VkContext::BeginSingleTimeCommands();

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

		VkContext::EndSingleTimeCommands(cmd_buffer);
	}

	void DeviceImage::copy_from_buffer(vk::Buffer buffer, u32 starting_layer, u32 layer_count)
	{
		vk::CommandBuffer cmd_buffer = VkContext::BeginSingleTimeCommands();

		vk::BufferImageCopy region;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = this->subresource_range.aspectMask;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = starting_layer;
		region.imageSubresource.layerCount = layer_count;

		region.imageOffset = {{ 0, 0, 0 }};
		region.imageExtent = this->extent;

		cmd_buffer.copyBufferToImage(
			buffer,
			this->img,
			vk::ImageLayout::eTransferDstOptimal,
			1, &region
		);

		VkContext::EndSingleTimeCommands(cmd_buffer);
	}

	void DeviceImage::copy_from_buffers(const vk::Buffer* buffers, u32 buffer_count, u32 starting_layer)
	{
		vk::CommandBuffer cmd_buffer = VkContext::BeginSingleTimeCommands();

		vk::BufferImageCopy region;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = this->subresource_range.aspectMask;

		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { { 0, 0, 0 } };
		region.imageExtent = this->extent;

		for (u32 i = starting_layer; i < starting_layer + buffer_count; i++)
		{
			region.imageSubresource.baseArrayLayer = i;

			cmd_buffer.copyBufferToImage(
				buffers[i],
				this->img,
				vk::ImageLayout::eTransferDstOptimal,
				1, &region
			);
		}

		VkContext::EndSingleTimeCommands(cmd_buffer);
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
		vk::Format format,
		u32 layer_count
	)
	{
		vk::ImageViewCreateInfo create_info;

		create_info.image = img;
		if (layer_count == 1)
			create_info.viewType = vk::ImageViewType::e2D;
		else if (layer_count > 1)
			create_info.viewType = vk::ImageViewType::e2DArray;
		else
			throw std::runtime_error("Failed to create Image View: Invalid layer count!");

		create_info.format = format;

		create_info.subresourceRange.aspectMask = aspect_mask;

		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;

		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = layer_count;

		return VkContext::GetLogicalDevice().createImageView(create_info);
	}
} // namespace Na
