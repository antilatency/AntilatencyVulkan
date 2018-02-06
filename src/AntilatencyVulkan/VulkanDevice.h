#pragma once

#include <map>
#include <cassert>

#include "AntilatencyVulkanCommon.h"
#include "Utils/TypeList.h"
#include "Functions/VulkanFunction.h"
#include "Functions/VulkanFunctionGroup.h"
#include "DeviceExtensions/DeviceExtension.h"


VulkanDeviceFunction(vkDestroyDevice) };
VulkanDeviceFunction(vkGetDeviceQueue) };

using VulkanDeviceFunctions = VulkanFunctionGroup<
	vkDestroyDevice,
	vkGetDeviceQueue
>;

class VulkanPhysicalDevice;

class VulkanDevice : public RefCounter {
	friend class Ref<VulkanDevice>;
	using ExtensionFunctionGroupContainer = std::map< std::string, Ref<VulkanFunctionGroupBase> >;

public:
	using FamilyIndex = uint32_t;
	using MaxQueuesAvailable = uint32_t;

public:
	template<class Extensions = NullType>
    static auto create(const Ref<VulkanPhysicalDevice>& physicalDevice,
                       const VkDevice device,
					   const std::vector<std::string>& enabledExtensions,
					   //const std::vector< std::pair<FamilyIndex, MaxQueuesAvailable> > enabledQueuesInFamilies,
					   const VulkanDeviceFunctions& functions) 
	{
		ExtensionFunctionGroupContainer extensionFunctions;
		
		if constexpr (!std::is_same<Extensions, NullType>::value) {
			extensionFunctions = constructDeviceExtensionFunctionGroups<Extensions>(physicalDevice, device, enabledExtensions);
		}

        auto constructedDevice = Ref<VulkanDevice>(new VulkanDevice(physicalDevice, device, extensionFunctions, /*enabledQueuesInFamilies,*/ functions));
		
        return constructedDevice;
	}

	auto getHandle() const {
		return _device;
	}

	template<class T>
	auto getExtension() {

		constexpr bool isDeviceExtension = std::is_base_of<DeviceExtension, T>::value;
		static_assert(isDeviceExtension, "T is not an DeviceExtension derived class");

		const auto extensionTypeName = T::extensionTypeIdStatic();

		if (_extensionsFunctions.count(extensionTypeName)) {
			auto& functionGroup = _extensionsFunctions.at(extensionTypeName);
			auto typeSpecificFunctionGroup = ref_static_cast<T::FunctionGroupType>(functionGroup);
			return T::create(Ref<VulkanDevice>(this), typeSpecificFunctionGroup.operator->());
		}

		return Ref<T>(nullptr);
	}

	auto getPhysicalDevice() {
		return _physicalDevice;
	}

	auto getQueue(uint32_t familyIndex, uint32_t queueIndex) {
		VkQueue queue;

		//auto foundEntry = std::find_if(_enabledQueuesInFamilies.begin(), _enabledQueuesInFamilies.end(), [=](const auto& pair) {
		//if family index is same and requested index is less than total available (ordered) queue.
		//	return pair.first == familyIndex && queueIndex < pair.second;
		//});

		//auto queueWasOrdered = (foundEntry != _enabledQueuesInFamilies.end());

		//if (queueWasOrdered) {
		assert(_functions.isAllFunctionsLoaded() && "Not all functions is loaded");

		_functions.get<vkGetDeviceQueue>().function(_device, familyIndex, queueIndex, &queue);
		return queue;
		//}
		//else {
		//	queue = VK_NULL_HANDLE;
		//	return queue;
		//}
	}

private:
	VulkanDevice(const Ref<VulkanPhysicalDevice> physicalDevice,
				 const VkDevice device,
		         const ExtensionFunctionGroupContainer& enabledExtensions,
                 //const std::vector< std::pair<FamilyIndex, MaxQueuesAvailable> > enabledQueuesInFamilies,
                 const VulkanDeviceFunctions& functions) :
        _device(device),
        _physicalDevice(physicalDevice),
		//_enabledQueuesInFamilies(enabledQueuesInFamilies),
        _functions(functions),
        _extensionsFunctions(enabledExtensions)
	{}


	template<class ExtensionList, std::size_t I>
	static auto constructDeviceExtensionFunctionGroups_impl(const Ref<VulkanPhysicalDevice> physicalDevice, const VkDevice deviceHandle, const std::vector<std::string>& extensions) {
		ExtensionFunctionGroupContainer functionGroups;

		using ExtensionType = typename type_at<I, ExtensionList>::type;

		constexpr bool isInstanceExtension = std::is_base_of<DeviceExtension, ExtensionType>::value;
		static_assert(isInstanceExtension, "ExtensionType is not an DeviceExtension derived class");

		using ExtensionFunctionGroup = typename ExtensionType::FunctionGroupType;

		if (ExtensionType::canBeCreated(extensions)) {
			Ref< ExtensionFunctionGroup > functionGroup(new ExtensionFunctionGroup);

			auto loaderFunction = physicalDevice->getDeviceLoaderFunction();
			functionGroup->load(loaderFunction, deviceHandle);

			functionGroups.insert(std::make_pair(ExtensionType::extensionTypeIdStatic(), ref_static_cast<VulkanFunctionGroupBase>(functionGroup)));
		}

		if constexpr (I > 0) {
			auto recursivelyObtainedFunctionGroups = constructDeviceExtensionFunctionGroups_impl<ExtensionList, I - 1>(physicalDevice, deviceHandle, extensions);
			functionGroups.insert(recursivelyObtainedFunctionGroups.begin(), recursivelyObtainedFunctionGroups.end());
		}

		return functionGroups;
	}


	template<class ExtensionList>
	static auto constructDeviceExtensionFunctionGroups(const Ref<VulkanPhysicalDevice> physicalDevice, const VkDevice deviceHandle, const std::vector<std::string>& extensions) {
		constexpr std::size_t const size = length<ExtensionList>::value;

		//constexpr auto extensionsRepeats = checkExtensionListForRepeats<ExtensionList, size - 1>();
		//static_assert(!extensionsRepeats, "You extension repeats in declaration. Remove repeated extensions");

		return constructDeviceExtensionFunctionGroups_impl<ExtensionList, size - 1>(physicalDevice, deviceHandle, extensions);
		//return ExtensionFunctionGroupContainer();
	}

private:
    VkDevice _device;
	Ref<VulkanPhysicalDevice> _physicalDevice;
	VulkanDeviceFunctions _functions;
    ExtensionFunctionGroupContainer _extensionsFunctions;
	//std::vector< std::pair<FamilyIndex, MaxQueuesAvailable> > _enabledQueuesInFamilies;
};
