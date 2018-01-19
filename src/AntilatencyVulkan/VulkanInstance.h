#pragma once
#include <memory>

#include "VulkanInstanceFactory.h"
#include "VulkanPhysicalDevice.h"

VulkanInstanceFunction(vkDestroyInstance) };
VulkanInstanceFunction(vkEnumeratePhysicalDevices) };
using VulkanInstanceFunctions = VulkanFunctionGroup<
	vkDestroyInstance,
	vkEnumeratePhysicalDevices
>;


class VulkanInstance : public RefCounter{
	friend class Ref<VulkanInstance>;
public:
	VkInstance instance = VK_NULL_HANDLE;
private:
	//using 
	const AbstractRef factory;
	VulkanInstanceFunctions instanceFunctions;

	VulkanPhysicalDeviceFunctions physicalDeviceFunctions;

	std::vector<VulkanPhysicalDevice> physicalDevices;

public:
	
	VulkanInstance(
		const AbstractRef& factory,
		VkInstance instance,
		const VulkanInstanceFunctions& instanceFunctions,
		const VulkanPhysicalDeviceFunctions& physicalDeviceFunctions
	) :
		factory(factory),
		instance(instance),
		instanceFunctions(instanceFunctions),
		physicalDeviceFunctions(physicalDeviceFunctions)
	{}
	

public:
	
	bool isAllFunctionsLoaded() {
		return
			instanceFunctions.isAllFunctionsLoaded() &&
			physicalDeviceFunctions.isAllFunctionsLoaded();
	}

	std::vector<VulkanPhysicalDevice> enumeratePhysicalDevices() {
		
		auto rawPhysicalDevices = vulkanEnumerate(
			instanceFunctions.get<vkEnumeratePhysicalDevices>().function,
			instance);


		std::vector<VulkanPhysicalDevice> physicalDevices(rawPhysicalDevices.size());
		for (int i = 0; i < physicalDevices.size(); i++) {
			physicalDevices[i] = VulkanPhysicalDevice(&physicalDeviceFunctions, rawPhysicalDevices[i]);
		}
		return physicalDevices;
	}



};

using VulkanInstanceRef = Ref<VulkanInstance>;
