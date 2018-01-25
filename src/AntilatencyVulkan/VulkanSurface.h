#pragma once

#include <AntilatencyVulkanCommon.h>
#include <VulkanPhysicalDevice.h>
#include "InstanceExtensions/SurfaceInstanceExtension.h"

class VulkanInstance;

//Declare Vulkan Surface functions 

//VulkanInstanceFunction(vkDestroySurfaceKHR) };
//VulkanInstanceFunction(vkGetPhysicalDeviceSurfaceSupportKHR) };
//VulkanInstanceFunction(vkGetPhysicalDeviceSurfaceCapabilitiesKHR) };
//VulkanInstanceFunction(vkGetPhysicalDeviceSurfaceFormatsKHR) };
//VulkanInstanceFunction(vkGetPhysicalDeviceSurfacePresentModesKHR) };
//
//
//using VulkanSurfaceFunctions = VulkanFunctionGroup<
//    vkDestroySurfaceKHR,
//    vkGetPhysicalDeviceSurfaceSupportKHR,
//    vkGetPhysicalDeviceSurfaceCapabilitiesKHR,
//    vkGetPhysicalDeviceSurfaceFormatsKHR,
//    vkGetPhysicalDeviceSurfacePresentModesKHR
//>;


//class VulkanSurface;
//using VulkanSurfaceRef = Ref<VulkanSurface>;


class VulkanSurface :  public RefCounter {
    friend Ref<VulkanSurface>;
public:
    static std::set<std::string> getInstanceExtensionName() {
        return std::set<std::string> { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
    }

    VulkanSurface(const Ref<VulkanInstance>& instanceRef, 
				  VkSurfaceKHR surface, 
				  const Ref<SurfaceInstanceExtension>& surfaceInstanceExtension) :
		_instanceRef(instanceRef),
		_surface(surface),
		_surfaceInstanceExtension(surfaceInstanceExtension)
	{}

	//auto getPhysicalDeviceFormats(const VulkanPhysicalDevice& physicalDevice) {
	//	auto functionPtr = _surfaceFunctions.get<vkGetPhysicalDeviceSurfaceFormatsKHR>().function;
	//	return vulkanEnumerate(
	//		_surfaceFunctions.get<vkGetPhysicalDeviceSurfaceFormatsKHR>().function,
	//		physicalDevice.physicalDevice,
	//		_surface);
	//}

private:
    VkSurfaceKHR _surface;
	Ref<VulkanInstance> _instanceRef;
	//FIXME: Референсить только группу функций??
	Ref<SurfaceInstanceExtension> _surfaceInstanceExtension;
};
