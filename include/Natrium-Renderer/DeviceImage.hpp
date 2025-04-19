#if !defined(NA_DEVICE_IMAGE_HPP)
#define NA_DEVICE_IMAGE_HPP

namespace Na {
	vk::Format FindSupportedFormat(
		const std::initializer_list<vk::Format>& candidates,
		vk::ImageTiling tiling,
		vk::FormatFeatureFlags features
	);

	class DeviceImage {
	public:
		vk::Image img;
		vk::DeviceMemory memory;

		union {
			vk::Extent3D extent;
			struct {
				u32 width, height, depth;
			};
		};
		vk::Format format;
		vk::ImageSubresourceRange subresource_range;

		DeviceImage(void)
		: img(nullptr), memory(nullptr), extent({}), format(vk::Format::eUndefined)
		{}
		DeviceImage(
			const vk::Extent3D& extent,
			vk::ImageAspectFlagBits aspect_mask,
			vk::Format format,
			vk::ImageTiling tiling,
			vk::ImageUsageFlags usage,
			vk::SharingMode sharing_mode,
			vk::SampleCountFlagBits sample_count,
			vk::MemoryPropertyFlags memory_properties
		);
		void destroy(void);

		void transition_layout(
			vk::ImageLayout old_layout,
			vk::ImageLayout new_layout,
			vk::CommandPool cmd_pool
		);

		void copy_from_buffer(vk::Buffer buffer, vk::CommandPool cmd_pool);
	};

	vk::ImageView CreateImageView(vk::Image img, vk::ImageAspectFlagBits aspect_mask, vk::Format format);
}

#endif // NA_DEVICE_IMAGE_HPP