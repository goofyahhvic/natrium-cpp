#if !defined(NA_VULKAN_HPP)
#define NA_VULKAN_HPP

#include "Natrium-Core/Core.hpp"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#define VK_CHECK(x) VkResult result = x; if (result) g_Logger.fmt(Na::Trace, "Vulkan Error: ", Na::VkResultToString(result))      

namespace Na {
	inline std::string_view VkResultToString(VkResult result)
	{
		#define STR(r) case VK_##r: return #r
		switch (result)
		{
			STR(NOT_READY);
			STR(TIMEOUT);
			STR(EVENT_SET);
			STR(EVENT_RESET);
			STR(INCOMPLETE);
			STR(ERROR_OUT_OF_HOST_MEMORY);
			STR(ERROR_OUT_OF_DEVICE_MEMORY);
			STR(ERROR_INITIALIZATION_FAILED);
			STR(ERROR_DEVICE_LOST);
			STR(ERROR_MEMORY_MAP_FAILED);
			STR(ERROR_LAYER_NOT_PRESENT);
			STR(ERROR_EXTENSION_NOT_PRESENT);
			STR(ERROR_FEATURE_NOT_PRESENT);
			STR(ERROR_INCOMPATIBLE_DRIVER);
			STR(ERROR_TOO_MANY_OBJECTS);
			STR(ERROR_FORMAT_NOT_SUPPORTED);
			STR(ERROR_FRAGMENTED_POOL);
			STR(ERROR_UNKNOWN);
			STR(ERROR_OUT_OF_POOL_MEMORY);
			STR(ERROR_INVALID_EXTERNAL_HANDLE);
			STR(ERROR_FRAGMENTATION);
			STR(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
			STR(PIPELINE_COMPILE_REQUIRED);
			STR(ERROR_SURFACE_LOST_KHR);
			STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
			STR(SUBOPTIMAL_KHR);
			STR(ERROR_OUT_OF_DATE_KHR);
			STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
			STR(ERROR_VALIDATION_FAILED_EXT);
			STR(ERROR_INVALID_SHADER_NV);
		#undef STR
		}
		return "UNKNOWN_ERROR";
	}

	struct QueueFamilyIndices {
		u32 graphics = UINT32_MAX;
		inline operator bool(void) const { return graphics != UINT32_MAX; }

		static QueueFamilyIndices Get(vk::PhysicalDevice device, vk::SurfaceKHR surface);
	};
} // namespace Na

#endif // NA_VULKAN_HPP