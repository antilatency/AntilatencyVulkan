#pragma once
#include "LibraryLoader.h"
#include "VulkanFunctionGroup.h"

#include "Functions/Instance/vkGetInstanceProcAddr.h"
#include "Functions/Instance/vkCreateInstance.h"
#include "Functions/Instance/vkEnumerateInstanceLayerProperties.h"
#include "Functions/Instance/vkEnumerateInstanceExtensionProperties.h"

#include "VulkanInstance.h"

class VulkanInstanceFactory: public VulkanFunctionGroup<
	vkGetInstanceProcAddr,
	vkCreateInstance,
	vkEnumerateInstanceLayerProperties,
	vkEnumerateInstanceExtensionProperties> {
private:
	Library library;
	friend class vkGetInstanceProcAddr;
public:
	//VulkanInstanceFactory() = delete;
	VulkanInstanceFactory(bool autoLoad = true) {
		if (autoLoad) load();
	}
public:
	void load() {
		library = Library("vulkan-1");
		this->loadAllFunctions(this,nullptr);
	}

	VulkanInstance<VulkanInstanceFactory> createInstanceDefault() {
		return VulkanInstance<VulkanInstanceFactory>(this, get<vkCreateInstance>().default());
	}
	
	VulkanInstance<VulkanInstanceFactory> createInstanceDebug() {
		return VulkanInstance<VulkanInstanceFactory>(this, get<vkCreateInstance>().debug());
	}

};