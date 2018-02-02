#pragma once

#include <AntilatencyVulkanCommon.h>
#include <VulkanPhysicalDevice.h>
#include "InstanceExtensions/SurfaceInstanceExtension.h"

class VulkanInstance;

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
	 
	auto getPhysicalDevicePresentQueueFamiliyIndices(const VulkanPhysicalDevice& physicalDevice) {
		std::vector<uint32_t> suitableFamilyIndices;

		auto availableFamilieProperties = physicalDevice.getQueueFamilyProperties();
		
		assert(_surfaceInstanceExtension->_functions->isAllFunctionsLoaded() && "vkGetPhysicalDeviceSurfaceSupportKHR is not loaded");

		auto checkForPresent = _surfaceInstanceExtension->_functions->get<vkGetPhysicalDeviceSurfaceSupportKHR>().function;

		for (std::size_t i = 0; i < availableFamilieProperties.size(); ++i ) {
			VkBool32 isSupported = false;
			
			auto result = checkForPresent(physicalDevice.getHandle(), i, _surface ,&isSupported);
			assert(result == VK_SUCCESS && "Could not check present support");

			if (isSupported) {
				suitableFamilyIndices.push_back(i);
			}
		}
		
		return suitableFamilyIndices;
	}
	
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
