#if !defined(NA_VULKAN_HPP)
#define NA_VULKAN_HPP

#include "Natrium/Core.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#define NA_VERIFY_VK(x, ...) NA_VERIFY(x == vk::Result::eSuccess, __VA_ARGS__)

namespace Na {
	struct QueueFamilyIndices {
		u32 graphics = UINT32_MAX;
		inline operator bool(void) const { return graphics != UINT32_MAX; }

		static QueueFamilyIndices Get(vk::PhysicalDevice device, vk::SurfaceKHR surface);
	};
} // namespace Na

#endif // NA_VULKAN_HPP