#pragma once
#include "VulkanPhysicalDeviceFunctions.h"
#include "VulkanFunctionGroup.h"


VulkanInstanceFunction(vkGetPhysicalDeviceFeatures) };
VulkanInstanceFunction(vkGetPhysicalDeviceFormatProperties) };
VulkanInstanceFunction(vkGetPhysicalDeviceImageFormatProperties) };
VulkanInstanceFunction(vkGetPhysicalDeviceProperties) };
VulkanInstanceFunction(vkGetPhysicalDeviceQueueFamilyProperties) };
VulkanInstanceFunction(vkGetPhysicalDeviceMemoryProperties) };
VulkanInstanceFunction(vkCreateDevice) };
VulkanInstanceFunction(vkEnumerateDeviceExtensionProperties) };
VulkanInstanceFunction(vkEnumerateDeviceLayerProperties) };

VulkanInstanceFunction(vkGetPhysicalDeviceDisplayPropertiesKHR) };
VulkanInstanceFunction(vkGetDisplayPlaneCapabilitiesKHR) };

using VulkanPhysicalDeviceFunctions = VulkanFunctionGroup<
	vkGetPhysicalDeviceFeatures,
	vkGetPhysicalDeviceFormatProperties,
	vkGetPhysicalDeviceImageFormatProperties,
	vkGetPhysicalDeviceProperties,
	vkGetPhysicalDeviceQueueFamilyProperties,
	vkGetPhysicalDeviceMemoryProperties,
	vkCreateDevice,
	vkEnumerateDeviceExtensionProperties,
	vkEnumerateDeviceLayerProperties,

	vkGetPhysicalDeviceDisplayPropertiesKHR,

	vkGetDisplayPlaneCapabilitiesKHR
>;



class VulkanPhysicalDevice {
private:
	VulkanPhysicalDeviceFunctions* functions;
	VkPhysicalDevice physicalDevice;
public:
	VulkanPhysicalDevice() {}
	VulkanPhysicalDevice(VulkanPhysicalDeviceFunctions* functions, VkPhysicalDevice physicalDevice): functions(functions), physicalDevice(physicalDevice) {
	
	}
	VkPhysicalDeviceFeatures getFeatures() {
		VkPhysicalDeviceFeatures result;
		functions->get<vkGetPhysicalDeviceFeatures>().function(physicalDevice, &result);
		return result;
	}
	VkPhysicalDeviceProperties getProperties() {
		VkPhysicalDeviceProperties result;
		functions->get<vkGetPhysicalDeviceProperties>().function(physicalDevice, &result);
		return result;
	}

	std::vector<VkExtensionProperties> getExtensionProperties(const char* layerName = nullptr) {
		auto function = functions->get<vkEnumerateDeviceExtensionProperties>().function;
		std::vector<VkExtensionProperties> result;
		uint32_t count = 0;
		if (VK_SUCCESS == function(physicalDevice, layerName, &count, nullptr)) {
			VkResult r;
			do {
				result.resize(count);
				r = function(physicalDevice, layerName, &count, result.data());
				if ((r != VK_SUCCESS) && (r != VK_INCOMPLETE)) return std::vector<VkExtensionProperties>();
			} while (r == VK_INCOMPLETE);

		}
		return result;
	}
	

	auto getDisplayPropertiesKHR() {
		auto function = functions->get<vkGetPhysicalDeviceDisplayPropertiesKHR>().function;
		std::vector<VkDisplayPropertiesKHR> result;
		uint32_t count = 0;
		if (VK_SUCCESS == function(physicalDevice, &count, nullptr)) {
			VkResult r;
			do {
				result.resize(count);
				r = function(physicalDevice, &count, result.data());
				if ((r != VK_SUCCESS) && (r != VK_INCOMPLETE)) return std::vector<VkDisplayPropertiesKHR>();
			} while (r == VK_INCOMPLETE);

		}
		return result;
	}


	/*VkDisplayPlaneCapabilitiesKHR getDisplayPlaneCapabilities() {
		VkDisplayPlaneCapabilitiesKHR result;
		auto function = functions->get<vkGetDisplayPlaneCapabilitiesKHR>().function;
		function(physicalDevice,)
		return result;
	}
*/


	/*vkGetPhysicalDeviceFeatures,
		vkGetPhysicalDeviceFormatProperties,
		vkGetPhysicalDeviceImageFormatProperties,
		
		vkGetPhysicalDeviceQueueFamilyProperties,
		vkGetPhysicalDeviceMemoryProperties,
		vkCreateDevice,
		vkEnumerateDeviceExtensionProperties,
		vkEnumerateDeviceLayerProperties*/
};