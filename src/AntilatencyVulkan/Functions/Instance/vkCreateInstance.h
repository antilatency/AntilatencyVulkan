#pragma once
#include "Functions/VulkanFunction.h"


class vkCreateInstance : public VulkanFunction<PFN_vkCreateInstance> {
public:
    const char* getName() override { return "vkCreateInstance"; }
	VkResult operator()(
		const VkInstanceCreateInfo*                 pCreateInfo,
		const VkAllocationCallbacks*                pAllocator,
		VkInstance*                                 pInstance) {
		return function(pCreateInfo, pAllocator, pInstance);
	}

	static VkInstanceCreateInfo defaultCreateInfo() {
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


	VkInstance default() {
		VkInstanceCreateInfo createInfo = defaultCreateInfo();
		VkInstance instance = nullptr;
		function(&createInfo, nullptr, &instance);
		return instance;
	}

	VkInstance debug() {
		VkInstanceCreateInfo createInfo = defaultCreateInfo();
        const char* layerNames[1] = { "VK_LAYER_LUNARG_standard_validation" };
		createInfo.enabledLayerCount = 1;
		createInfo.ppEnabledLayerNames = layerNames;

		VkInstance instance = nullptr;
		function(&createInfo, nullptr, &instance);
		return instance;
	}


	template<typename Host>
	void load(Host* host, void*) {
		function = (Prototype)host->get<vkGetInstanceProcAddr>()(nullptr, getName());
	}

};
