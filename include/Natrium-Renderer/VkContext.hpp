#if !defined(NA_VK_CONTEXT_HPP)
#define NA_VK_CONTEXT_HPP

#include "Vulkan.hpp"
#include "Natrium-Core/Core.hpp"

#include "Natrium-Core/Template/ArrayVector.hpp"

namespace Na {
    inline constexpr bool k_ValidationLayersEnabled = k_BuildConfig != BuildConfig::Distribution;

	struct QueueFamilyIndices {
		u32 graphics = UINT32_MAX;
		inline operator bool(void) const { return graphics != UINT32_MAX; }

		static QueueFamilyIndices Get(vk::PhysicalDevice device, vk::SurfaceKHR surface);
	};
	struct SurfaceSupport {
		VkSurfaceCapabilitiesKHR capabilities;
		Na::ArrayVector<vk::SurfaceFormatKHR> formats;
		Na::ArrayVector<vk::PresentModeKHR> present_modes;

		inline operator bool(void) const { return formats.size() && present_modes.size(); }

		static SurfaceSupport Get(vk::SurfaceKHR surface, vk::PhysicalDevice device);
	};

	class VkContext {
	public:
		VkContext(void) = default;

		VkContext(const VkContext& other) = delete;
		VkContext& operator=(const VkContext& other) = delete;

		VkContext(VkContext&& other);
		VkContext& operator=(VkContext&& other);

		static VkContext Initialize(void);
		static void Shutdown(void);

		[[nodiscard]] static inline vk::Instance               GetInstance(void)       { return s_Context->m_Instance; }
		[[nodiscard]] static inline vk::DebugUtilsMessengerEXT GetDebugMessenger(void) { return s_Context->m_DebugMessenger; }
		[[nodiscard]] static inline vk::PhysicalDevice         GetPhysicalDevice(void) { return s_Context->m_PhysicalDevice; }
		[[nodiscard]] static inline vk::Device                 GetLogicalDevice(void)  { return s_Context->m_LogicalDevice; }

		[[nodiscard]] static inline vk::Queue                  GetGraphicsQueue(void)  { return s_Context->m_GraphicsQueue; }

	private:
		vk::Instance               m_Instance = nullptr;
		vk::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
		vk::PhysicalDevice         m_PhysicalDevice = nullptr;
		vk::Device                 m_LogicalDevice = nullptr;

		vk::Queue                  m_GraphicsQueue = nullptr;

		static inline VkContext* s_Context = nullptr;
	};
} // namespace Na

#endif // NA_VK_CONTEXT_HPP