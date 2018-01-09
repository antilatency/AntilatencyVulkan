#pragma once
#include "Functions/VulkanFunction.h"


class vkGetInstanceProcAddr : public VulkanFunction<PFN_vkGetInstanceProcAddr> {
public:
    const char* getName() override { return "vkGetInstanceProcAddr"; }
	template<typename Context>
	void load(Context* context) {
		function = (Prototype)context->library.getFunction(getName());
	}
	PFN_vkVoidFunction operator()(VkInstance instance, const char* pName) {
		return function(instance, pName);
	}
};
