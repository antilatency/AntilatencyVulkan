#pragma once
#include "AntilatencyVulkanCommon.h"



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

	auto getExtensionProperties(const char* layerName = nullptr) {
		return vulkanEnumerate(
			functions->get<vkEnumerateDeviceExtensionProperties>().function,
			physicalDevice, layerName);
	}

	auto getQueueFamilyProperties() {
		return vulkanEnumerate(
			functions->get<vkGetPhysicalDeviceQueueFamilyProperties>().function,
			physicalDevice);
	}

	//GetPhysicalDeviceQueueFamilyProperties

	
	void createDevice() {
		/*VkDevice device;
		auto function = functions->get<vkCreateDevice>().function;
		VkDeviceCreateInfo createInfo;
		createInfo.
		function(physicalDevice,)*/
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