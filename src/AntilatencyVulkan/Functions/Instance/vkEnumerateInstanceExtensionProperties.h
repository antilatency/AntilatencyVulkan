#pragma once
#include "Functions/VulkanFunction.h"


class vkEnumerateInstanceExtensionProperties : public VulkanFunction<PFN_vkEnumerateInstanceExtensionProperties> {
public:
	char* getName() override { return "vkEnumerateInstanceExtensionProperties"; }

	template<typename Host>
	void load(Host* host, void*) {
		function = (Prototype)host->get<vkGetInstanceProcAddr>()(nullptr, getName());
	}
};