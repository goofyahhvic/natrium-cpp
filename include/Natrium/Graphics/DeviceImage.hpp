#if !defined(NA_DEVICE_IMAGE_HPP)
#define NA_DEVICE_IMAGE_HPP

#include "Natrium/Core.hpp"

namespace Na {
	vk::Format FindSupportedFormat(
		const std::initializer_list<vk::Format>& candidates,
		vk::ImageTiling tiling,
		vk::FormatFeatureFlags features
	);

	class DeviceImage {
	public:
		vk::Image img = nullptr;
		vk::DeviceMemory memory = nullptr;

		union {
			vk::Extent3D extent;
			struct {
				u32 width, height, depth;
			};
		};
		vk::Format format = vk::Format::eUndefined;
		vk::ImageSubresourceRange subresource_range;

		DeviceImage(void) {};
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

		DeviceImage(const DeviceImage& other) = delete;
		DeviceImage& operator=(const DeviceImage& other) = delete;

		DeviceImage(DeviceImage&& other);
		DeviceImage& operator=(DeviceImage&& other);

		void transition_layout(vk::ImageLayout old_layout, vk::ImageLayout new_layout);

		void copy_from_buffer(vk::Buffer buffer);

		[[nodiscard]] inline operator bool(void) const { return format != vk::Format::eUndefined; }
	};

	vk::ImageView CreateImageView(vk::Image img, vk::ImageAspectFlagBits aspect_mask, vk::Format format);
}

#endif // NA_DEVICE_IMAGE_HPP