#pragma once
#include "Functions/VulkanFunction.h"

#define VulkanInstanceFactoryFunction(Name) \
class Name : public VulkanFunction<PFN_##Name> {\
public:\
	const char* getName() override { return #Name; }\
	\
	void load(PFN_vkGetInstanceProcAddr getInstanceProcAddr) {\
		function = (Prototype)getInstanceProcAddr(nullptr, getName());\
	}\

VulkanInstanceFactoryFunction(vkCreateInstance) };
VulkanInstanceFactoryFunction(vkEnumerateInstanceLayerProperties) };
VulkanInstanceFactoryFunction(vkEnumerateInstanceExtensionProperties) };
using VulkanInstanceFactoryFunctions = VulkanFunctionGroup<
	vkCreateInstance,
	vkEnumerateInstanceLayerProperties,
	vkEnumerateInstanceExtensionProperties
>;



#define VulkanInstanceFunction(Name) \
class Name : public VulkanFunction<PFN_##Name> {\
public:\
	const char* getName() override { return #Name; }\
	\
	void load(PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkInstance instance) {\
		function = (Prototype)getInstanceProcAddr(instance, getName());\
	}\


VulkanInstanceFunction(vkDestroyInstance) };
VulkanInstanceFunction(vkEnumeratePhysicalDevices) };
using VulkanInstanceFunctions = VulkanFunctionGroup<
	vkDestroyInstance,
	vkEnumeratePhysicalDevices
>;

