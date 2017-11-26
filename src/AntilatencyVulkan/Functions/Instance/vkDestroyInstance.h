#pragma once
#include "Functions/VulkanFunction.h"

class vkDestroyInstance : public VulkanFunction<PFN_vkDestroyInstance> {
public:
    const char* getName() override { return "vkDestroyInstance"; }

	template<typename Context>
	void load(Context* context, void* instance) {
		function = (Prototype)context->get<vkGetInstanceProcAddr>()((VkInstance)instance, getName());
	}

};
