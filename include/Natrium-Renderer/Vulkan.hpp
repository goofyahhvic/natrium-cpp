#if !defined(NA_VULKAN_HPP)
#define NA_VULKAN_HPP

#include "Natrium-Core/Core.hpp"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#define NA_CHECK_VK(x, ...) { vk::Result r = x; if (r != vk::Result::eSuccess) throw std::runtime_error(NA_FORMAT("Result {}: {}", (int)r, NA_FORMAT(__VA_ARGS__))); }

namespace Na {
	struct QueueFamilyIndices {
		u32 graphics = UINT32_MAX;
		inline operator bool(void) const { return graphics != UINT32_MAX; }

		static QueueFamilyIndices Get(vk::PhysicalDevice device, vk::SurfaceKHR surface);
	};
} // namespace Na

#endif // NA_VULKAN_HPP