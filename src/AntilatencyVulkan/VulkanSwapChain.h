#pragma once

#include "AntilatencyVulkanCommon.h"
#include "VulkanDevice.h"


VulkanDeviceFunction(vkCreateSwapchainKHR) };
VulkanDeviceFunction(vkDestroySwapchainKHR) };
VulkanDeviceFunction(vkGetSwapchainImagesKHR) };
VulkanDeviceFunction(vkAcquireNextImageKHR) };
VulkanDeviceFunction(vkQueuePresentKHR) };


using SwapChainFunctions = VulkanFunctionGroup<
	vkCreateSwapchainKHR,
	vkDestroySwapchainKHR,
	vkGetSwapchainImagesKHR,
	vkAcquireNextImageKHR,
	vkQueuePresentKHR
>;


class VulkanSwapChain : public RefCounter {
	friend class Ref<VulkanSwapChain>;

public:
	static auto create(VkSwapchainKHR swapchainHandle, const Ref<VulkanDevice>& deviceRef, SwapChainFunctions* functions) {
		return Ref<VulkanSwapChain>(new VulkanSwapChain(swapchainHandle, deviceRef, functions));
	}

private:
	VulkanSwapChain(VkSwapchainKHR swapchainHandle, const Ref<VulkanDevice>& deviceRef,  SwapChainFunctions* functions) :
		_swapchainHandle(swapchainHandle),
		_deviceRef(deviceRef),
		_functions(functions)
	{}

private:
	VkSwapchainKHR _swapchainHandle;
	
	//We can not use SwapChainExtension in that place, because SwapChain and SwapChainExtension references each other in include hierarchy.
	Ref<VulkanDevice> _deviceRef;
	SwapChainFunctions* _functions;
};
