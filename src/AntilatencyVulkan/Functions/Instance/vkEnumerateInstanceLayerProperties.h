#pragma once
#include "Functions/VulkanFunction.h"
#include <vector>

class vkEnumerateInstanceLayerProperties : public VulkanFunction<PFN_vkEnumerateInstanceLayerProperties> {
public:
    const char* getName() override { return "vkEnumerateInstanceLayerProperties"; }
	
	template<typename Host>
	void load(Host* host, void*) {
		function = (Prototype)host->get<vkGetInstanceProcAddr>()(nullptr, getName());
	}

	std::vector<VkLayerProperties> asVector() {
		std::vector<VkLayerProperties> result;
		uint32_t count = 0;
		if (VK_SUCCESS == function(&count, nullptr)) {
			VkResult r;
			do{
				result.resize(count);
				r = function(&count, result.data());
				if ((r != VK_SUCCESS) && (r != VK_INCOMPLETE)) return std::vector<VkLayerProperties>();
			} while (r==VK_INCOMPLETE);
			
		}
		return result;

	
	}
};
