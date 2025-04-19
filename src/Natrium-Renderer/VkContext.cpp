#include "Pch.hpp"
#include "Natrium-Renderer/VkContext.hpp"

#include "Natrium-Core/Logger.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT types,
	const VkDebugUtilsMessengerCallbackDataEXT* data,
	void* user_data
)
{
	if (Na::k_ValidationLayersEnabled)
	{
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			Na::g_Logger(Na::Error, data->pMessage);
		else
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			Na::g_Logger(Na::Warn, data->pMessage);
		else
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			Na::g_Logger(Na::Info, data->pMessage);
		else
			Na::g_Logger(Na::Trace, data->pMessage);
	}

	return VK_FALSE;
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;

}
static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func)
		func(instance, debugMessenger, pAllocator);
}

namespace Na {
	static Na::ArrayList<const char*> extensions;
	static Na::ArrayList<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	static Na::ArrayList<const char*> requiredDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
		//VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME
	};

	static void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info)
	{
		create_info.messageSeverity =
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;

		create_info.messageType =
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

		create_info.setPfnUserCallback(debugCallback);
	}

	static bool validationLayersSupported(void)
	{
		auto available_layers = vk::enumerateInstanceLayerProperties();

		for (const char* requested_layer : validationLayers)
		{
			bool found = false;

			for (const VkLayerProperties& available_layer : available_layers)
				if (!strcmp(requested_layer, available_layer.layerName))
				{
					found = true;
					break;
				}

			if (!found)
				return false;
		}

		return true;
	}

	QueueFamilyIndices QueueFamilyIndices::Get(vk::PhysicalDevice device, vk::SurfaceKHR surface)
	{
		auto properties = device.getQueueFamilyProperties();

		QueueFamilyIndices indices;
		for (u32 i = 0; const auto& property : properties)
		{
			if (property.queueFlags & vk::QueueFlagBits::eCompute)
				if (device.getSurfaceSupportKHR(i, surface))
					indices.graphics = i;

			if (indices)
				break;

			i++;
		}

		return indices;
	}

	static bool areRequiredDeviceExtensionsSupported(vk::PhysicalDevice device)
	{
		auto available_extensions = device.enumerateDeviceExtensionProperties();
		Na::ArrayList<std::string_view> required_extensions(requiredDeviceExtensions.capacity());

		for (const char* required_extension : requiredDeviceExtensions)
		{
			required_extensions.emplace(required_extension);
			for (const VkExtensionProperties& available_extension : available_extensions)
				if (required_extensions.tail() == available_extension.extensionName)
					required_extensions.pop();
		}

		return required_extensions.empty();
	}

	SurfaceSupport SurfaceSupport::Get(vk::SurfaceKHR surface, vk::PhysicalDevice device)
	{
		SurfaceSupport support{};

		support.capabilities = device.getSurfaceCapabilitiesKHR(surface);

		u32 format_count;
		(void)device.getSurfaceFormatsKHR(surface, &format_count, nullptr);
		support.formats.resize(format_count);
		(void)device.getSurfaceFormatsKHR(surface, &format_count, support.formats.ptr());

		u32 present_mode_count;
		(void)device.getSurfacePresentModesKHR(surface, &present_mode_count, nullptr);
		support.present_modes.resize(format_count);
		(void)device.getSurfacePresentModesKHR(surface, &format_count, support.present_modes.ptr());

		return support;
	}

	static i32 ratePhysicalDevice(vk::PhysicalDevice device, vk::SurfaceKHR surface)
	{
		if (!device)
			return 0;

		vk::PhysicalDeviceProperties properties = device.getProperties();
		vk::PhysicalDeviceFeatures features = device.getFeatures();

		if (!features.geometryShader)
			return 0;

		if (!features.samplerAnisotropy)
			return 0;

		if (!QueueFamilyIndices::Get(device, surface))
			return 0;

		if (!areRequiredDeviceExtensionsSupported(device))
			return 0;

		if (!SurfaceSupport::Get(surface, device))
			return 0;

		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			properties.limits.maxImageDimension2D += 1000;

		return properties.limits.maxImageDimension2D;
	}

	static vk::Instance createInstance(void)
	{
		if (k_ValidationLayersEnabled && !validationLayersSupported())
			throw std::runtime_error("Validation layers requested, but not supported!");

		vk::ApplicationInfo app_info;
		app_info.apiVersion = VK_API_VERSION_1_0;

		vk::InstanceCreateInfo create_info;
		create_info.pApplicationInfo = &app_info;

		u32 required_extension_count = 0;
		const char** required_extensions = glfwGetRequiredInstanceExtensions(&required_extension_count);

		extensions.reallocate(required_extension_count);
		for (u32 i = 0; i < required_extension_count; i++)
			extensions.emplace(required_extensions[i]);

		vk::DebugUtilsMessengerCreateInfoEXT debug_create_info;
		if (k_ValidationLayersEnabled)
		{
			extensions.emplace(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			create_info.enabledLayerCount = (u32)validationLayers.size();
			create_info.ppEnabledLayerNames = validationLayers.ptr();

			populateDebugMessengerCreateInfo(debug_create_info);
			create_info.pNext = &debug_create_info;
		} else
		{
			create_info.enabledLayerCount = 0;
		}

		create_info.enabledExtensionCount = (u32)extensions.size();
		create_info.ppEnabledExtensionNames = extensions.ptr();

		return vk::createInstance(create_info);
	}

	static vk::DebugUtilsMessengerEXT createDebugMessenger(vk::Instance instance)
	{
		if (!k_ValidationLayersEnabled)
			return nullptr;

		vk::DebugUtilsMessengerCreateInfoEXT create_info;
		populateDebugMessengerCreateInfo(create_info);

		VkDebugUtilsMessengerEXT messenger;
		VK_CHECK(CreateDebugUtilsMessengerEXT(instance, (VkDebugUtilsMessengerCreateInfoEXT*)&create_info, nullptr, &messenger));
		return messenger;
	}

	static vk::PhysicalDevice pickPhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface)
	{
		auto devices = instance.enumeratePhysicalDevices();

		vk::PhysicalDevice chosen_device = nullptr;

		i32 high_score = 0;
		for (const auto& device : devices)
		{
			i32 score = ratePhysicalDevice(device, surface);
			if (score > high_score)
			{
				high_score = score;
				chosen_device = device;
			}
		}

		return chosen_device;
	}

	static const float priorities[] = { 1.0f };
	static vk::DeviceQueueCreateInfo createQueueCreateInfo(u32 index, u32 count = 1, const float* _priorities = priorities)
	{
		vk::DeviceQueueCreateInfo create_info;
		create_info.queueFamilyIndex = index;
		create_info.queueCount = count;
		create_info.pQueuePriorities = _priorities;
		return create_info;
	}

	static vk::Device createLogicalDevice(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface, vk::Queue& queue)
	{
		QueueFamilyIndices indices = QueueFamilyIndices::Get(physical_device, surface);
		Na::ArrayList<vk::DeviceQueueCreateInfo> queue_create_infos;
		queue_create_infos.emplace(createQueueCreateInfo(indices.graphics));

		vk::DeviceCreateInfo create_info;

		create_info.queueCreateInfoCount = (u32)queue_create_infos.size();
		create_info.pQueueCreateInfos = queue_create_infos.ptr();

		vk::PhysicalDeviceFeatures device_features{};
		device_features.samplerAnisotropy = VK_TRUE;
		create_info.pEnabledFeatures = &device_features;

		create_info.enabledExtensionCount = (u32)requiredDeviceExtensions.size();
		create_info.ppEnabledExtensionNames = requiredDeviceExtensions.ptr();

		vk::Device device = physical_device.createDevice(create_info);
		queue = device.getQueue(indices.graphics, 0);

		return device;
	}

	VkContext VkContext::Initialize(void)
	{
		VkContext context;

		g_Logger(Info, "Initializing vulkan!");

		context.m_Instance = createInstance();
		context.m_DebugMessenger = createDebugMessenger(context.m_Instance);

		GLFWwindow* temp_window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
		VkSurfaceKHR temp_surface;
		VK_CHECK(glfwCreateWindowSurface(context.m_Instance, temp_window, nullptr, &temp_surface));

		context.m_PhysicalDevice = pickPhysicalDevice(context.m_Instance, temp_surface);
		context.m_LogicalDevice = createLogicalDevice(context.m_PhysicalDevice, temp_surface, context.m_GraphicsQueue);

		vkDestroySurfaceKHR(context.m_Instance, temp_surface, nullptr);
		glfwDestroyWindow(temp_window);

		s_Context = &context;
		return context;
	}

	void VkContext::Shutdown(void)
	{
		if (s_Context->m_LogicalDevice)
			s_Context->m_LogicalDevice.destroy();

		if (s_Context->m_DebugMessenger)
			DestroyDebugUtilsMessengerEXT(s_Context->m_Instance, s_Context->m_DebugMessenger, nullptr);

		if (s_Context->m_Instance)
			s_Context->m_Instance.destroy();

		s_Context = nullptr;
	}

	VkContext::VkContext(VkContext&& other)
	{
		memcpy(this, &other, sizeof(VkContext));
		memset(&other, 0, sizeof(VkContext));

		s_Context = this;
	}

	VkContext& VkContext::operator=(VkContext&& other)
	{
		memcpy(this, &other, sizeof(VkContext));
		memset(&other, 0, sizeof(VkContext));

		s_Context = this;
		return *this;
	}
} // namespace Na
