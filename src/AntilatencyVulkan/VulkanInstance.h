#pragma once
#include "VulkanInstanceFactory.h"

#include "Functions/Instance/vkDestroyInstance.h"

template<typename InstanceFactory>
class VulkanInstance : public VulkanFunctionGroup<vkDestroyInstance>{
public:
	InstanceFactory* factory = nullptr;
	VkInstance instance = nullptr;
public:
	constexpr VulkanInstance() {}

	VulkanInstance(InstanceFactory& factory, bool debug = false) : factory(&factory) {
		if (debug)
			instance = this->factory->get<vkCreateInstance>().debug();
		else
			instance = this->factory->get<vkCreateInstance>().default();
		if (instance) loadAllFunctions(this->factory, instance);
	}
private:
	VulkanInstance(const VulkanInstance&) = default;// = delete;
	VulkanInstance& operator=(const VulkanInstance&) = default;
	
public:
	VulkanInstance(VulkanInstance&& r) {
		static constexpr VulkanInstance zero = VulkanInstance();
		*this = r;
		r = zero;
	}
	VulkanInstance& operator=(VulkanInstance&& r) {
		static constexpr VulkanInstance zero = VulkanInstance();
		*this = r;
		r = zero;
		return *this;
	}

	VulkanInstance(InstanceFactory* factory , VkInstance instance, bool autoLoad = true): factory(factory), instance(instance){
		if (autoLoad) loadAllFunctions(factory, instance);
	}





};