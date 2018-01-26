#pragma once
#include <memory>
#include <cassert>
#include <map>

#include "VulkanInstanceFactory.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"
#include "InstanceExtensions/InstanceExtension.h"
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


class VulkanInstance : public RefCounter{
    friend class Ref<VulkanInstance>;
	
	using InstanceExtensionContainer = std::vector< Ref<InstanceExtension> >;
	using VulkanNativeExtensionsContainer = std::vector< std::string >;
	using InstanceExtensionFunctionGroupsContainer = std::map<std::string, Ref<VulkanFunctionGroupBase> >;

public:
    VkInstance _instance = VK_NULL_HANDLE;
private:
    //using

	const AbstractRef _factory;
    VulkanInstanceFunctions _instanceFunctions;

    VulkanPhysicalDeviceFunctions _physicalDeviceFunctions;

    std::vector<VulkanPhysicalDevice> _physicalDevices;
	
	VulkanNativeExtensionsContainer _vulkanExtensions;

	InstanceExtensionFunctionGroupsContainer _functionGroups;

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
        _factory(factory),
		_instance(instance),
		_vulkanExtensions(vkExtensions),
		_instanceFunctions(instanceFunctions),
		_physicalDeviceFunctions(physicalDeviceFunctions)
    {
		if constexpr(!std::is_same<Extensions, NullType>::value) {
			_functionGroups = constructInstanceExtensionFunctionGroups<Extensions>(vkExtensions);
		}
	}


public:

    bool isAllFunctionsLoaded() {
        return
			_instanceFunctions.isAllFunctionsLoaded() &&
			_physicalDeviceFunctions.isAllFunctionsLoaded();
    }

    std::vector<VulkanPhysicalDevice> enumeratePhysicalDevices() {

        auto rawPhysicalDevices = vulkanEnumerate(
			_instanceFunctions.get<vkEnumeratePhysicalDevices>().function,
            _instance);


        std::vector<VulkanPhysicalDevice> physicalDevices(rawPhysicalDevices.size());
        for (int i = 0; i < physicalDevices.size(); i++) {
            physicalDevices[i] = VulkanPhysicalDevice(&_physicalDeviceFunctions, rawPhysicalDevices[i]);
        }
        return physicalDevices;
    }


	template<class T>
	auto getExtension() {
		constexpr bool isInstanceExtension = std::is_base_of<InstanceExtension, T>::value;
		static_assert(isInstanceExtension, "T is not an InstanceExtension derived class");

		const auto extensionTypeName = T::extensionTypeIdStatic();

		if (_functionGroups.count(extensionTypeName)) {
			auto& functionGroup = _functionGroups.at(extensionTypeName);
			auto typeSpecificFunctionGroup = ref_static_cast<T::FunctionGroupType>(functionGroup);
			return T::create(Ref<VulkanInstance>(this), typeSpecificFunctionGroup.operator->());
		}

		return Ref<T>(nullptr);
	}


	auto getFactory() {
		return _factory;
	}


	template<class ExtensionList, std::size_t I>
	auto constructInstanceExtensions_impl(const std::vector<std::string>& extensions) {
		InstanceExtensionFunctionGroupsContainer functionGroups;

		using ExtensionType = typename type_at<I, ExtensionList>::type;
		using ExtensionFunctionGroup = ExtensionType::FunctionGroupType;

		if (ExtensionType::canBeCreated(extensions)) {
			Ref< ExtensionFunctionGroup > functionGroup(new ExtensionFunctionGroup);

			auto loaderFunction = ref_static_cast<VulkanInstanceFactory>(getFactory())->getLoaderFunction();
			functionGroup->load(loaderFunction, _instance);

			functionGroups.insert(std::make_pair(ExtensionType::extensionTypeIdStatic(), ref_static_cast<VulkanFunctionGroupBase>(functionGroup)));
		}

		if constexpr (I > 0) {
			auto recursivelyObtainedFunctionGroups = constructInstanceExtensions_impl<ExtensionList, I - 1>(extensions);
			functionGroups.insert(recursivelyObtainedFunctionGroups.begin(), recursivelyObtainedFunctionGroups.end());
		}

		return functionGroups;
	}


	template<class ExtensionList>
	auto constructInstanceExtensionFunctionGroups(const std::vector<std::string>& extensions) {
		constexpr std::size_t const size = length<ExtensionList>::value;

		//constexpr auto extensionsRepeats = checkExtensionListForRepeats<ExtensionList, size - 1>();
		//static_assert(!extensionsRepeats, "You extension repeats in declaration. Remove repeated extensions");

		return constructInstanceExtensions_impl<ExtensionList, size - 1>(extensions);
	}
};
