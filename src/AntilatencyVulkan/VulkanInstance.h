#pragma once
#include "VulkanInstanceFactory.h"

#include "Functions/Instance/vkDestroyInstance.h"

template<typename InstanceFactory>
class VulkanInstance : public VulkanFunctionGroup<vkDestroyInstance>{
public:
	InstanceFactory* factory = nullptr;
	VkInstance instance = nullptr;
public:
	VulkanInstance() {}

	VulkanInstance(const VulkanInstance&) = delete;
	VulkanInstance& operator=(const VulkanInstance&) = delete;

	void move(VulkanInstance&& r) {
		instance = r.instance;
		factory = r.factory;

		r.instance = nullptr;
		r.factory = nullptr;
	}

	VulkanInstance(VulkanInstance&& r) {
		move(std::move(r));
	}
	VulkanInstance& operator=(VulkanInstance&& r) {
		move(std::move(r));
		return *this;
	}

	VulkanInstance(InstanceFactory* factory , VkInstance instance, bool autoLoad = true): factory(factory), instance(instance){
		load(factory, instance);
	}





};