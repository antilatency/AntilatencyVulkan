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

	


	/*VkInstance default() {
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
	}*/


	void load(PFN_vkGetInstanceProcAddr loaderFunction) {
		function = (Prototype)loaderFunction(nullptr, getName());
	}

};
