#pragma once

#include <memory>
#include <cassert>
#include <map>
#include <algorithm>
#include <type_traits>


#include "Utils/TypeList.h"
#include "InstanceExtensions/InstanceExtension.h"
#include "VulkanPhysicalDevice.h"


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

class VulkanInstanceFactory;

class VulkanInstance : public RefCounter{

    friend class Ref<VulkanInstance>;
	using InstanceExtensionFunctionGroupsContainer = std::map<std::string, Ref<VulkanFunctionGroupBase> >;

public:
	template<class Extensions = NullType>
	static auto create(const AbstractRef& factory,
		const VkInstance instance,
		const std::vector<std::string>& enabledVulkanExtensions,
		const VulkanInstanceFunctions& instanceFunctions,
		const VulkanPhysicalDeviceFunctions& physicalDeviceFunctions)
	{
		InstanceExtensionFunctionGroupsContainer functionGroups;

        if constexpr (!std::is_same<Extensions, NullType>::value) {
			functionGroups = constructInstanceExtensionFunctionGroups<Extensions>(factory, instance, enabledVulkanExtensions);
		}

		return Ref<VulkanInstance>(new VulkanInstance(factory, instance, enabledVulkanExtensions, instanceFunctions, physicalDeviceFunctions, functionGroups));
	}


public:

    bool isAllFunctionsLoaded() {
        return
			_instanceFunctions.isAllFunctionsLoaded() &&
			_physicalDeviceFunctions.isAllFunctionsLoaded();
    }

    auto enumeratePhysicalDevices() {
		auto rawPhysicalDevices = vulkanEnumerate(
			_instanceFunctions.get<vkEnumeratePhysicalDevices>().function,
            _instance);

        std::vector< Ref<VulkanPhysicalDevice> > physicalDevices;

        for (const auto& rawPhysicalDevice : rawPhysicalDevices) {
            physicalDevices.push_back(VulkanPhysicalDevice::create(Ref<VulkanInstance>(this), &_physicalDeviceFunctions, rawPhysicalDevice));
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


	auto getFactory() const {
		return ref_static_cast<VulkanInstanceFactory>(_factory);
	}

	auto getHandle() const {
		return _instance;
	}

private:
	template<class ExtensionList, std::size_t I>
	static auto constructInstanceExtensions_impl(const AbstractRef& factory, const VkInstance instance, const std::vector<std::string>& extensions) {
		InstanceExtensionFunctionGroupsContainer functionGroups;

		using ExtensionType = typename type_at<I, ExtensionList>::type;

		constexpr bool isInstanceExtension = std::is_base_of<InstanceExtension, ExtensionType>::value;
		static_assert(isInstanceExtension, "ExtensionType is not an InstanceExtension derived class");

		using ExtensionFunctionGroup = typename ExtensionType::FunctionGroupType;

		if (ExtensionType::canBeCreated(extensions)) {
			Ref< ExtensionFunctionGroup > functionGroup(new ExtensionFunctionGroup);

			auto loaderFunction = ref_static_cast<VulkanInstanceFactory>(factory)->getLoaderFunction();
			functionGroup->load(loaderFunction, instance);

			functionGroups.insert(std::make_pair(ExtensionType::extensionTypeIdStatic(), ref_static_cast<VulkanFunctionGroupBase>(functionGroup)));
		}

		if constexpr (I > 0) {
			auto recursivelyObtainedFunctionGroups = constructInstanceExtensions_impl<ExtensionList, I - 1>(factory, instance, extensions);
			functionGroups.insert(recursivelyObtainedFunctionGroups.begin(), recursivelyObtainedFunctionGroups.end());
		}

		return functionGroups;
	}


	template<class ExtensionList>
	static auto constructInstanceExtensionFunctionGroups(const AbstractRef& factory, const VkInstance instance, const std::vector<std::string>& extensions) {
		constexpr std::size_t const size = length<ExtensionList>::value;

		//constexpr auto extensionsRepeats = checkExtensionListForRepeats<ExtensionList, size - 1>();
		//static_assert(!extensionsRepeats, "You extension repeats in declaration. Remove repeated extensions");

		return constructInstanceExtensions_impl<ExtensionList, size - 1>(factory, instance, extensions);
	}


	template<class Extensions = NullType>
	VulkanInstance(
		const AbstractRef& factory,
		const VkInstance instance,
		const std::vector<std::string> enabledVulkanExtensions,
		const VulkanInstanceFunctions& instanceFunctions,
		const VulkanPhysicalDeviceFunctions& physicalDeviceFunctions,
		const InstanceExtensionFunctionGroupsContainer& functionGroups
	) :
		_factory(factory),
		_instance(instance),
		_instanceFunctions(instanceFunctions),
		_physicalDeviceFunctions(physicalDeviceFunctions),
		_functionGroups(functionGroups)
	{}

private:
	VkInstance _instance = VK_NULL_HANDLE;
	const AbstractRef _factory;

	VulkanInstanceFunctions _instanceFunctions;
	VulkanPhysicalDeviceFunctions _physicalDeviceFunctions;

	InstanceExtensionFunctionGroupsContainer _functionGroups;
};
