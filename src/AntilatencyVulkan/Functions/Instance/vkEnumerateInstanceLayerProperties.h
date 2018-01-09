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

	/*std::vector<VkLayerProperties> asVector() {
		

	
	}*/
};
