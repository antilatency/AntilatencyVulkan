#pragma once
#include <vector>

#include "LibraryLoader.h"
#include "Functions/VulkanFunctionGroup.h"

/*
#include "Functions/Instance/vkGetInstanceProcAddr.h"
#include "Functions/Instance/vkCreateInstance.h"
#include "Functions/Instance/vkEnumerateInstanceLayerProperties.h"
#include "Functions/Instance/vkEnumerateInstanceExtensionProperties.h"*/

#include "VulkanInstance.h"

VulkanInstanceFunction(vkCreateInstance) };
VulkanInstanceFunction(vkEnumerateInstanceLayerProperties) };
VulkanInstanceFunction(vkEnumerateInstanceExtensionProperties) };

using VulkanInstanceFactoryFunctions = VulkanFunctionGroup<
	vkCreateInstance,
	vkEnumerateInstanceLayerProperties,
	vkEnumerateInstanceExtensionProperties
>;


class VulkanInstanceFactory :public RefCounter {
private:
	Library library;
	PFN_vkGetInstanceProcAddr loaderFunction;

    VulkanInstanceFactoryFunctions functions;

	VulkanInstanceFactory() {

#if defined(_WIN32)
        const char* libraryName = "vulkan-1";
#elif defined(__ANDROID__) || defined(__linux__)
        const char* libraryName = "libvulkan";
#endif
        library = Library(libraryName);
		loaderFunction = (PFN_vkGetInstanceProcAddr)library.getFunction("vkGetInstanceProcAddr");
		functions.load(loaderFunction);
	}

	//Only Ref<VulkanInstanceFactory> can destroy VulkanInstanceFactory
	friend class Ref<VulkanInstanceFactory>; 
	~VulkanInstanceFactory() override {}

public:
	static auto create() {
		return Ref<VulkanInstanceFactory>(new VulkanInstanceFactory());
	}

	std::vector<VkLayerProperties> enumerateLayerProperties() {
		return vulkanEnumerate(functions.get<vkEnumerateInstanceLayerProperties>().function);
	}

	std::vector<VkExtensionProperties> enumerateExtensionProperties() {
		return vulkanEnumerate(functions.get<vkEnumerateInstanceExtensionProperties>().function, nullptr);
	}
	

	static const VkInstanceCreateInfo getDefaultCreateInfo() {
		VkInstanceCreateInfo result;
		result.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		result.pNext = nullptr;
		result.flags = 0;
		result.pApplicationInfo = nullptr;
		result.enabledLayerCount = 0;
		result.ppEnabledLayerNames = nullptr;
		result.enabledExtensionCount = 0;
		result.ppEnabledExtensionNames = nullptr;
		return result;
	}

	static const VkInstanceCreateInfo getDebugCreateInfo() {
		VkInstanceCreateInfo result = getDefaultCreateInfo();
		static const char* layerNames[1] = { "VK_LAYER_LUNARG_standard_validation" };
		result.enabledLayerCount = 1;
		result.ppEnabledLayerNames = layerNames;
		return result;
	}



	auto createInstance(const VkInstanceCreateInfo& createInfo, const VkAllocationCallbacks* pAllocator = nullptr) {
		VkInstance instance;
		functions.get<vkCreateInstance>().function(&createInfo, pAllocator, &instance);

		VulkanInstanceFunctions instanceFunctions;
		instanceFunctions.load(loaderFunction, instance);

		VulkanPhysicalDeviceFunctions physicalDeviceFunctions;
		physicalDeviceFunctions.load(loaderFunction, instance);


		return Ref<VulkanInstance>(new VulkanInstance(AbstractRef(this), instance, instanceFunctions, physicalDeviceFunctions));

		//return VulkanInstance<VulkanInstanceFactory>::create(Ref<VulkanInstanceFactory>(this), instance);

	}



	/*VkInstance createInstance() {
		
	}
	
	VkInstance createInstanceDebug() {
		return functions.get<vkCreateInstance>().debug();
	}*/

};
