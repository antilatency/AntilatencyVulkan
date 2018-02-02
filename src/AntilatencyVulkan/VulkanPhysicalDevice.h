#pragma once

#include "AntilatencyVulkanCommon.h"
#include "QueueConstructor.h"
#include "QueueGarden.h"
//#include "VulkanDevice.h"


VulkanInstanceFunction(vkGetPhysicalDeviceFeatures) };
VulkanInstanceFunction(vkGetPhysicalDeviceFormatProperties) };
VulkanInstanceFunction(vkGetPhysicalDeviceImageFormatProperties) };
VulkanInstanceFunction(vkGetPhysicalDeviceProperties) };
VulkanInstanceFunction(vkGetPhysicalDeviceQueueFamilyProperties) };
VulkanInstanceFunction(vkGetPhysicalDeviceMemoryProperties) };
VulkanInstanceFunction(vkCreateDevice) };
VulkanInstanceFunction(vkEnumerateDeviceExtensionProperties) };
VulkanInstanceFunction(vkEnumerateDeviceLayerProperties) };

VulkanInstanceFunction(vkGetPhysicalDeviceDisplayPropertiesKHR) };
VulkanInstanceFunction(vkGetDisplayPlaneCapabilitiesKHR) };

using VulkanPhysicalDeviceFunctions = VulkanFunctionGroup<
	vkGetPhysicalDeviceFeatures,					// +
	vkGetPhysicalDeviceFormatProperties,			// 
	vkGetPhysicalDeviceImageFormatProperties,		//
	vkGetPhysicalDeviceProperties,					// +
	vkGetPhysicalDeviceQueueFamilyProperties,		// +
	vkGetPhysicalDeviceMemoryProperties,			//
	vkCreateDevice,									//
	vkEnumerateDeviceExtensionProperties,			// +
	vkEnumerateDeviceLayerProperties,				// +

	vkGetPhysicalDeviceDisplayPropertiesKHR,		//

	vkGetDisplayPlaneCapabilitiesKHR				//
>;


class VulkanInstance;


class VulkanPhysicalDevice : public RefCounter {
	friend class Ref<VulkanPhysicalDevice>;

public:
	static auto create(const Ref<VulkanInstance>& instanceRef, VulkanPhysicalDeviceFunctions* functions, const VkPhysicalDevice physicalDevice) {
		return Ref<VulkanPhysicalDevice>(new VulkanPhysicalDevice(instanceRef, functions, physicalDevice));
	}

	VkPhysicalDeviceFeatures getFeatures() const {
		VkPhysicalDeviceFeatures result;
		_functions->get<vkGetPhysicalDeviceFeatures>().function(_physicalDevice, &result);
		return result;
	}

	VkPhysicalDeviceProperties getProperties() const {
		VkPhysicalDeviceProperties result;
		_functions->get<vkGetPhysicalDeviceProperties>().function(_physicalDevice, &result);
		return result;
	}

	auto getExtensionProperties(const char* layerName = nullptr) const {
		return vulkanEnumerate(
			_functions->get<vkEnumerateDeviceExtensionProperties>().function,
			_physicalDevice, layerName);
	}

	auto getLayerProperties() const {
		return vulkanEnumerate(
			_functions->get<vkEnumerateDeviceLayerProperties>().function,
			_physicalDevice);
	}

	auto getQueueFamilyProperties() const {
		return vulkanEnumerate(
			_functions->get<vkGetPhysicalDeviceQueueFamilyProperties>().function,
			_physicalDevice);
	}

	auto getHandle() const {
		return _physicalDevice;
	}

	//TODO: ¬озвращать некий QueueDestribution с перегруженным оператором bool
	//template<class ...Constructors>
	//auto distributeQueues( Constructors&&... constructors ) const {
	//	
	//	auto constructorsTuple = std::forward_as_tuple(constructors...);

	//	
	//	using ConstructorsTupleType = decltype(constructorsTuple);
	//	constexpr auto tupleSize = std::tuple_size<ConstructorsTupleType>::value;
	//	
	//	//Fill all constructor types, if they all may contain same family indices
	//	for_each_in_tuple(constructorsTuple, [this](auto& constructor) {
	//		constructor.fill(*this);
	//	});
	//	
	//	//Remove family indices from constructors, that can not hold the same ones.

	//	return true;
	//}

	auto distributeQueues(std::vector<QueueConstructor*>& constructors) {
		//Fill all queue constructors as if the all can be the same
		/*for (QueueConstructor* c : constructors) {
			c->fill(*this);
		}*/

		QueueGarden garden(constructors, *this, getQueueFamilyProperties());
		auto queueList = garden.getQueueList();

		std::sort(queueList.begin(), queueList.end());

		do {
			auto ql = queueList;
			bool secusess = true;

			while (ql.size()) {
				auto queue = ql.back();
				ql.pop_back();

				if (garden.addQueue(queue) == false) {
					secusess = false;
					break;
				}
			}

			//all queues is distributed
			if (secusess) {
				for (const auto* q : queueList) {
					auto constructor = q->owner;
					constructor->_queueIndexes.insert(QueueConstructor::QueueIndexPair(q->familyIndex, q->queueIndex));
				}
				return true;
			}
			
			garden.clear();
		} while (std::next_permutation(queueList.begin(), queueList.end()));
		
		return false;
	}


	
	auto beginDeviceCreation() {
		/*VkDevice device;
		auto function = functions->get<vkCreateDevice>().function;
		VkDeviceCreateInfo createInfo;
		createInfo.
		function(physicalDevice,)*/
	}

	//template<class Extensions = NullType>
	//auto createDevice(const VkDeviceCreateInfo& createInfo) {
	//	VkDevice device;

	//	if (functions->get<vkCreateDevice>().function(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
	//		return Ref<VulkanDevice>(nullptr);
	//	}

	//	std::vector<std::string> enabledExtensions(createInfo.ppEnabledExtensionNames, createInfo.ppEnabledExtensionNames + createInfo.enabledExtensionCount);
	//	
	//	std::vector< std::pair<uint32_t, uint32_t> > queues;

	//	for (int i = 0; i < createInfo.queueCreateInfoCount; i++) {
	//		const auto& queueCreateInfo = createInfo.pQueueCreateInfos[i];
	//		queues.push_back(std::make_pair(queueCreateInfo->queueFamilyIndex, queueCreateInfo->queueCount));
	//	}

	//	return VulkanDevice::create<Extensions>(device, enabledExtensions, queues, functions);
	//}



	/*VkDisplayPlaneCapabilitiesKHR getDisplayPlaneCapabilities() {
		VkDisplayPlaneCapabilitiesKHR result;
		auto function = functions->get<vkGetDisplayPlaneCapabilitiesKHR>().function;
		function(physicalDevice,)
		return result;
	}
*/


	/*vkGetPhysicalDeviceFeatures,
		vkGetPhysicalDeviceFormatProperties,
		vkGetPhysicalDeviceImageFormatProperties,
		
		vkGetPhysicalDeviceQueueFamilyProperties,
		vkGetPhysicalDeviceMemoryProperties,
		vkCreateDevice,
		vkEnumerateDeviceExtensionProperties,
		vkEnumerateDeviceLayerProperties*/
private:
	VulkanPhysicalDevice(const Ref<VulkanInstance>& instanceRef, VulkanPhysicalDeviceFunctions* functions, const VkPhysicalDevice physicalDevice) :
		_functions(functions),
		_physicalDevice(physicalDevice),
		_instanceRef(instanceRef)
	{}

private:
	VulkanPhysicalDeviceFunctions * _functions;
	VkPhysicalDevice _physicalDevice;
	Ref<VulkanInstance> _instanceRef;
};