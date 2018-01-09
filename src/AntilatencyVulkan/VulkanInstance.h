#pragma once
#include <memory>

#include "VulkanInstanceFactory.h"

#include "Functions/Instance/VulkanInstanceFunctions.h"
#include "VulkanPhysicalDevice.h"


class VulkanInstance :public RefCounter{
	friend class Ref<VulkanInstance>;
private:
	//using 
	const AbstractRef factory;
	VkInstance instance = nullptr;
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

	void updatePhysicalDevices() {
	
	}


	bool isAllFunctionsLoaded() {
		return
			instanceFunctions.isAllFunctionsLoaded() &&
			physicalDeviceFunctions.isAllFunctionsLoaded();
	}

	std::vector<VulkanPhysicalDevice> enumeratePhysicalDevices() {
		auto function = instanceFunctions.get<vkEnumeratePhysicalDevices>().function;
		std::vector<VkPhysicalDevice> rawPhysicalDevices;
		
		uint32_t count = 0;
		if (VK_SUCCESS == function(instance, &count, nullptr)) {
			VkResult result;
			do {
				rawPhysicalDevices.resize(count);
				result = function(instance, &count, rawPhysicalDevices.data());
				if ((result != VK_SUCCESS) && (result != VK_INCOMPLETE)) return std::vector<VulkanPhysicalDevice>();
			} while (result == VK_INCOMPLETE);

		}

		std::vector<VulkanPhysicalDevice> physicalDevices(rawPhysicalDevices.size());
		for (int i = 0; i < physicalDevices.size(); i++) {
			physicalDevices[i] = VulkanPhysicalDevice(&physicalDeviceFunctions, rawPhysicalDevices[i]);
		}
		return physicalDevices;
	}



};