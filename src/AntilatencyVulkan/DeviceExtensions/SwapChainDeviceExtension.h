#pragma once

#include <array>

#include "AntilatencyVulkanCommon.h"
#include "VulkanDevice.h"

#include "DeviceExtensions/DeviceExtension.h"


VulkanDeviceFunction(vkCreateSwapchainKHR) };
VulkanDeviceFunction(vkDestroySwapchainKHR) };
VulkanDeviceFunction(vkGetSwapchainImagesKHR) };
VulkanDeviceFunction(vkAcquireNextImageKHR) };
VulkanDeviceFunction(vkQueuePresentKHR) };


using SwapChainDeviceExtensionFunctions = VulkanFunctionGroup<
	vkCreateSwapchainKHR,
	vkDestroySwapchainKHR,
	vkGetSwapchainImagesKHR,
	vkAcquireNextImageKHR,
	vkQueuePresentKHR
>;


class SwapChainDeviceExtension : public DeviceExtension {
	friend class Ref<SwapChainDeviceExtension>;
public:
	using FunctionGroupType = SwapChainDeviceExtensionFunctions;

	constexpr static auto requiredExtensionNamesStatic() {
		constexpr std::array<const char*, 1> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		return extensions;
	}

	static auto canBeCreated(const std::vector<std::string>& extensions) {
		for (const auto& requiredExt : requiredExtensionNamesStatic()) {
			auto extensionFound = std::find(extensions.begin(), extensions.end(), std::string(requiredExt)) != extensions.end();

			if (extensionFound == false) {
				return false;
			}
		}
		return true;
	}

	//TODO: write compile time hashing
	static std::string extensionTypeIdStatic() {
		return "SwapChainDeviceExtension";
	}

public:
	static auto create(const Ref<VulkanDevice>& device, SwapChainDeviceExtensionFunctions * functions) {
		return Ref<SwapChainDeviceExtension>(new SwapChainDeviceExtension(device, functions));
	}

	//We assume that we create swapchain from 
	auto createSwapChain(const VkSwapchainCreateInfoKHR& createInfo) {
		//createInfo.
	}

private:
	SwapChainDeviceExtension(const Ref<VulkanDevice>& device, SwapChainDeviceExtensionFunctions* functions) :
		DeviceExtension(device),
		_functions(functions)
	{}

private:
	SwapChainDeviceExtensionFunctions * _functions;
};