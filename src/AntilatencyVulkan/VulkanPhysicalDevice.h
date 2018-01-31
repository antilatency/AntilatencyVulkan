#pragma once

#include "AntilatencyVulkanCommon.h"
#include "QueueConstructor.h"
#include "QueueGarden.h"



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



class VulkanPhysicalDevice {
	friend class VulkanSurface;
private:
	VulkanPhysicalDeviceFunctions* functions;
	VkPhysicalDevice physicalDevice;
public:
	VulkanPhysicalDevice() {}
	VulkanPhysicalDevice(VulkanPhysicalDeviceFunctions* functions, VkPhysicalDevice physicalDevice): functions(functions), physicalDevice(physicalDevice) {
	
	}

	VkPhysicalDeviceFeatures getFeatures() const {
		VkPhysicalDeviceFeatures result;
		functions->get<vkGetPhysicalDeviceFeatures>().function(physicalDevice, &result);
		return result;
	}

	VkPhysicalDeviceProperties getProperties() const {
		VkPhysicalDeviceProperties result;
		functions->get<vkGetPhysicalDeviceProperties>().function(physicalDevice, &result);
		return result;
	}

	auto getExtensionProperties(const char* layerName = nullptr) const {
		return vulkanEnumerate(
			functions->get<vkEnumerateDeviceExtensionProperties>().function,
			physicalDevice, layerName);
	}

	auto getLayerProperties() const {
		return vulkanEnumerate(
			functions->get<vkEnumerateDeviceLayerProperties>().function,
			physicalDevice);
	}

	auto getQueueFamilyProperties() const {
		return vulkanEnumerate(
			functions->get<vkGetPhysicalDeviceQueueFamilyProperties>().function,
			physicalDevice);
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

	auto distributeQueues(std::vector<QueueConstructor*>& constructors) const {
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


	auto createDevice() {
		/*VkDevice device;
		auto function = functions->get<vkCreateDevice>().function;
		VkDeviceCreateInfo createInfo;
		createInfo.
		function(physicalDevice,)*/
	}



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
};