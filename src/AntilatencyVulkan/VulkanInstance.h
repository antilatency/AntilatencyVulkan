#pragma once
#include <memory>
#include <cassert>

#include "VulkanInstanceFactory.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"
#include "InstanceExtensions/InstanceExtensionTags.h"
#include "Utils/TypeList.h"

VulkanInstanceFunction(vkDestroyInstance) };
VulkanInstanceFunction(vkEnumeratePhysicalDevices) };

using VulkanInstanceFunctions = VulkanFunctionGroup<
    vkDestroyInstance,
    vkEnumeratePhysicalDevices
>;


//template<class ExtensionList, class ExtensionType, std::size_t I>
//constexpr bool checkExtensionListForRepeats_helper() {
//	using ExtensionTypeForCheck = typename type_at<I, ExtensionList>::type;
//
//	if constexpr (ExtensionType::extensionTypeIdStatic() == ExtensionTypeForCheck::extensionTypeIdStatic()) {
//		return true;
//	}
//	else if constexpr (I > 0){
//		return checkExtensionListForRepeats_helper<ExtensionList, ExtensionType, I - 1>();
//	}
//	else {
//		return false;
//	}
//}
//
//template<class ExtensionList, std::size_t I>
//constexpr bool checkExtensionListForRepeats() {
//	using ExtensionType = typename type_at<I, ExtensionList>::type;
//	constexpr auto typeRepeats = checkExtensionListForRepeats_helper<ExtensionList, ExtensionType, I - 1>();
//
//	if constexpr (typeRepeats == true) {
//		return true;
//	}
//	else if constexpr( I > 0 ){
//		return checkExtensionListForRepeats<ExtensionList, I - 1>();
//	}
//	return false;
//}


template<class ExtensionList, std::size_t I>
auto getExt_imp(const std::vector<std::string>& extensions, Ref<VulkanInstance>& vulkanInstance) {
	std::vector< Ref<InstanceExtension> > instanceExtensions;

	using ExtensionType = typename type_at<I, ExtensionList>::type;

	if (ExtensionType::canBeCreated(extensions)) {
		auto instanceExtensionRef = ref_static_cast<InstanceExtension>(ExtensionType::create(vulkanInstance));
		instanceExtensions.push_back(instanceExtensionRef);
	}

	if constexpr (I > 0) {
		auto anotherFuncCallExtensions = getExt_imp<ExtensionList, I - 1>(extensions, vulkanInstance);
		instanceExtensions.insert(instanceExtensions.end(), anotherFuncCallExtensions.begin(), anotherFuncCallExtensions.end());
	}

	return instanceExtensions;
}


template<class ExtensionList>
auto getExt(const std::vector<std::string>& extensions, Ref<VulkanInstance>& vulkanInstance) {
	
	constexpr std::size_t const size = length<ExtensionList>::value;

	//auto extensionsRepeats = checkExtensionListForRepeats<ExtensionList, size - 1>();
	//static_assert(!extensionsRepeats, "You extension repeats in declaration. Remove repeated extensions");

	return getExt_imp<ExtensionList, size - 1>(extensions, vulkanInstance);
}


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

	template<class Extensions = NullType>
    VulkanInstance(
        const AbstractRef& factory,
        VkInstance instance,
		std::vector<std::string> vkExtensions,
        const VulkanInstanceFunctions& instanceFunctions,
        const VulkanPhysicalDeviceFunctions& physicalDeviceFunctions,
		Extensions
    ) :
        factory(factory),
        instance(instance),
		vulkanExtensions(vkExtensions),
        instanceFunctions(instanceFunctions),
        physicalDeviceFunctions(physicalDeviceFunctions)
    {
		if constexpr(!std::is_same<Extensions, NullType>::value) {
			enabledExtensions = getExt<Extensions>(vkExtensions, Ref<VulkanInstance>(this));
		}
	}


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


	template<class ExtensionList>
	std::vector< Ref<InstanceExtension> > example(const std::vector<std::string>& extensions) {

		std::vector< Ref<InstanceExtension> > instanceExtensions;
		constexpr auto max = length<ExtensionList>::value;

		for (auto i = 0; i < max; i++) {
			if (type_at<i, ExtensionList>::type::canBeCreated(extensions)) {
				instanceExtensions.push_back(Ref<InstanceExtension>(type_at<i, ExtensionList>::type::create()));
			}
		}

		return instanceExtensions;
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

		assert(false && "Suitable instance extension is not declared in create info ");
	}

	auto getFactory() {
		return factory;
	}
};
