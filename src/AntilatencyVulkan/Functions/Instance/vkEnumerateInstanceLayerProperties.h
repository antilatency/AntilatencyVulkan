#pragma once
#include "Functions/VulkanFunction.h"


class vkEnumerateInstanceLayerProperties : public VulkanFunction<PFN_vkEnumerateInstanceLayerProperties> {
public:
	char* getName() override { return "vkEnumerateInstanceLayerProperties"; }
	
	template<typename Host>
	void load(Host* host, void*) {
		function = (Prototype)host->get<vkGetInstanceProcAddr>()(nullptr, getName());
	}
};