#pragma once

#include <array>

#include "InstanceExtensions/InstanceExtension.h"

#include "AntilatencyVulkanCommon.h"
#include "VulkanInstance.h"


VulkanInstanceFunction(vkDestroySurfaceKHR) };
VulkanInstanceFunction(vkGetPhysicalDeviceSurfaceSupportKHR) };
VulkanInstanceFunction(vkGetPhysicalDeviceSurfaceCapabilitiesKHR) };
VulkanInstanceFunction(vkGetPhysicalDeviceSurfaceFormatsKHR) };
VulkanInstanceFunction(vkGetPhysicalDeviceSurfacePresentModesKHR) };


using SurfaceInstanceExtensionFunctions = VulkanFunctionGroup<
	vkDestroySurfaceKHR,
	vkGetPhysicalDeviceSurfaceSupportKHR,
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR,
	vkGetPhysicalDeviceSurfaceFormatsKHR,
	vkGetPhysicalDeviceSurfacePresentModesKHR
>;


class SurfaceInstanceExtension : public InstanceExtension {	
	friend class Ref<SurfaceInstanceExtension>;
public:
	using FunctionGroupType = SurfaceInstanceExtensionFunctions;

	constexpr static auto requiredExtensionNamesStatic() {
		constexpr std::array<const char*, 1> extensions = { VK_KHR_SURFACE_EXTENSION_NAME };
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
	static InstanceExensionTypeId extensionTypeIdStatic() {
		return "SurfaceInstanceExtension";
	}

	//TODO: write compile time hashing
	InstanceExensionTypeId extensionTypeId() override {
		return "SurfaceInstanceExtension";
	}

public:
	static auto create(const Ref<VulkanInstance>& vulkanInstance, SurfaceInstanceExtensionFunctions* functions) {
		return Ref<SurfaceInstanceExtension>( new SurfaceInstanceExtension(vulkanInstance, functions));
	}

private:
	SurfaceInstanceExtension(const Ref<VulkanInstance>& vulkanInstance, SurfaceInstanceExtensionFunctions* functions) :
		InstanceExtension(vulkanInstance),
		_functions(functions)
	{}

private:
	SurfaceInstanceExtensionFunctions * _functions;
};