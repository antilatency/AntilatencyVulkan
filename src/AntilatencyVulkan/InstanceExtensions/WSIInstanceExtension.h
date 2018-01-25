#pragma once

#include "InstanceExtensions/InstanceExtensionTags.h"
#include "VulkanSurface.h"

#include <array>

#ifdef VK_USE_PLATFORM_WIN32_KHR

VulkanInstanceFunction(vkCreateWin32SurfaceKHR) };

using WSIInstanceExtensionFunctions = VulkanFunctionGroup<
	vkCreateWin32SurfaceKHR
>;

#else
#	error unsupported os
#endif


class WSIInstanceExtension final : public InstanceExtension {
	friend class Ref<WSIInstanceExtension>;
public:
	using FunctionGroupType = WSIInstanceExtensionFunctions;
public:
	constexpr static auto requiredExtensionNamesStatic() {
		constexpr std::array<const char*, 2> extensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
		return extensions;
	}

	static auto canBeCreated(const std::vector<std::string>& extensions) {
		for (const auto& requiredExt : requiredExtensionNamesStatic() ) {
			auto extensionFound = std::find(extensions.begin(), extensions.end(), std::string(requiredExt)) != extensions.end();

			if (extensionFound == false) {
				return false;
			}
		}
		return true;
	}

	//static auto create(Ref<VulkanInstance>& vulkanInstance) {
	//	auto loaderFunction = ref_static_cast<VulkanInstanceFactory>(vulkanInstance->getFactory())->getLoaderFunction();
	//	
	//	WSIInstanceExtensionFunctions functions;
	//	functions.load(loaderFunction, vulkanInstance->instance);

	//	return Ref<WSIInstanceExtension>( new WSIInstanceExtension(vulkanInstance, functions) );
	//}

	//TODO: write compile time hashing
	static InstanceExensionTypeId extensionTypeIdStatic() {
		return "WSIInstanceExtension";
	}

	//TODO: write compile time hashing
	InstanceExensionTypeId extensionTypeId() override {
		return "WSIInstanceExtension";
	}

	~WSIInstanceExtension() override = default;

#ifdef VK_USE_PLATFORM_WIN32_KHR
	Ref<VulkanSurface> createSurface(HINSTANCE hinstance, HWND hwnd) {
        auto loaderFunction = ref_static_cast<VulkanInstanceFactory>(_instanceRef->getFactory())->getLoaderFunction();

		VkWin32SurfaceCreateInfoKHR createInfo = {};

		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hinstance = hinstance;
		createInfo.hwnd = hwnd;

		VkSurfaceKHR surface;
		auto createSurfaceFunc = _functions->get<vkCreateWin32SurfaceKHR>().function;
		createSurfaceFunc(_instanceRef->_instance, &createInfo, nullptr, &surface);

        VulkanSurfaceFunctions surfaceFunctions;
		surfaceFunctions.load(loaderFunction, _instanceRef->_instance);

		return Ref<VulkanSurface>(new VulkanSurface(_instanceRef, surface, surfaceFunctions));
	}
#else
	Ref<VulkanSurface> createSurface() {
		static_assert(false, "Not implemented");
	}
#endif

public:
	WSIInstanceExtension(const Ref<VulkanInstance>& vulkanInstance, WSIInstanceExtensionFunctions* functions) :
		InstanceExtension(vulkanInstance),
		_functions(functions)
	{}

private:
	WSIInstanceExtensionFunctions* _functions;
};
