#pragma once
#include <memory>
#include <cassert>

#include "VulkanInstanceFactory.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"
#include "InstanceExtensions/InstanceExtensionTags.h"

VulkanInstanceFunction(vkDestroyInstance) };
VulkanInstanceFunction(vkEnumeratePhysicalDevices) };

using VulkanInstanceFunctions = VulkanFunctionGroup<
    vkDestroyInstance,
    vkEnumeratePhysicalDevices
>;


class VulkanInstance : public RefCounter{
    friend class Ref<VulkanInstance>;
	
	using InstanceExtensionContainer = std::vector< Ref<InstanceExtension> >;
	using VulkanNativeExtensionsContainer = std::vector< std::string >;

public:
    VkInstance instance = VK_NULL_HANDLE;
private:
    //using
    const AbstractRef factory;
    VulkanInstanceFunctions instanceFunctions;

    VulkanPhysicalDeviceFunctions physicalDeviceFunctions;

    std::vector<VulkanPhysicalDevice> physicalDevices;

	InstanceExtensionContainer enabledExtensions;
	VulkanNativeExtensionsContainer vulkanExtensions;

public:

    VulkanInstance(
        const AbstractRef& factory,
        VkInstance instance,
		std::vector<std::string> vkExtensions,
        const VulkanInstanceFunctions& instanceFunctions,
        const VulkanPhysicalDeviceFunctions& physicalDeviceFunctions,
        PFN_vkGetInstanceProcAddr loaderFunc
    ) :
        factory(factory),
        instance(instance),
		vulkanExtensions(vkExtensions),
        instanceFunctions(instanceFunctions),
        physicalDeviceFunctions(physicalDeviceFunctions)
    {}


public:

    bool isAllFunctionsLoaded() {
        return
            instanceFunctions.isAllFunctionsLoaded() &&
            physicalDeviceFunctions.isAllFunctionsLoaded();
    }

    std::vector<VulkanPhysicalDevice> enumeratePhysicalDevices() {

        auto rawPhysicalDevices = vulkanEnumerate(
            instanceFunctions.get<vkEnumeratePhysicalDevices>().function,
            instance);


        std::vector<VulkanPhysicalDevice> physicalDevices(rawPhysicalDevices.size());
        for (int i = 0; i < physicalDevices.size(); i++) {
            physicalDevices[i] = VulkanPhysicalDevice(&physicalDeviceFunctions, rawPhysicalDevices[i]);
        }
        return physicalDevices;
    }

	template<class T>
	auto get() {
		constexpr bool isInstanceExtension = std::is_base_of<InstanceExtension, T>::value;
		static_assert(isInstanceExtension, "T is not an InstanceExtension derived class");

		for (const auto& enabledExtension : enabledExtensions) {
			if (enabledExtension->extensionTypeId() == T::extensionTypeIdStatic()) {
				return ref_static_cast<T>(enabledExtension);
			}
		}

		//assert(false && "Suitable instance extension is not declared in create info ");

		//if not found -> create
		auto requiredExtensions = T::requiredExtensionNamesStatic();
		for (const auto& e : requiredExtensions) {
			auto extensionEnabled = std::find(vulkanExtensions.begin(), vulkanExtensions.end(), std::string(e)) != vulkanExtensions.end();
			if (extensionEnabled == false) {
				assert(false && "Suitable instance extension is not enabled");
			}
		}

		auto e = T::create(Ref<VulkanInstance>(this));
		enabledExtensions.push_back(ref_static_cast<InstanceExtension>(e));
		return e;
	}

	auto getFactory() {
		return factory;
	}
};
