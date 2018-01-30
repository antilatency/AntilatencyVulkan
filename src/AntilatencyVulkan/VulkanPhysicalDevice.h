#pragma once

#include "AntilatencyVulkanCommon.h"
#include "MPLUtils.h"
#include "QueueConstructor.h"



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

	auto distributeQueues(std::vector<QueueConstructor*> constructors) const {
		//Fill all queue constructors as if the all can be the same
		for (QueueConstructor* c : constructors) {
			c->fill(*this);
		}

		//Find intersection of every constructor with each other
		using ConstructorIndicesPair = std::pair<QueueConstructor::FamilyIndex, QueueConstructor::FamilyIndex>;
		std::map<ConstructorIndicesPair, QueueConstructor::QueuFamilyIndexContainer> intersections;


		//Fill intersections between constructors
		assert(constructors.size() >= 2);

		for (int i = 0; i < constructors.size(); i++) {
			for (int j = i + 1; j < constructors.size(); j++) {
				auto familyIndices = constructors[i]->findIntersections(*constructors[j]);
				intersections.insert(std::make_pair(ConstructorIndicesPair(i, j), familyIndices));
			}
		}

		////remove intersections from each constructors' set
		for (int i = 0; i < constructors.size(); i++) {
			for (int j = i + 1; j < constructors.size(); j++) {
				//If intersection fas found
				if (intersections.count(ConstructorIndicesPair(i, j))) {
					auto intersection = intersections.at(ConstructorIndicesPair(i, j));
					if (intersection.size() > 0) {
						constructors[i]->removeQueueIndexes(intersection);
						constructors[j]->removeQueueIndexes(intersection);
					}
				}
			}
		}

		////Extend constructors' sets with intersections to satisfy minimum requirements.
		for (int i = 0; i < constructors.size(); i++) {
			for (int j = i + 1; j < constructors.size(); j++) {
				//if intersection was found
				if (intersections.count(ConstructorIndicesPair(i, j))) {
					auto& intersection = intersections.at(ConstructorIndicesPair(i, j));
					if (constructors[i]->canBeSame(*constructors[j])) {
						if (constructors[i]->_queueFamilyIndices.size() < constructors[i]->_minRequested) {
							constructors[i]->expandWithQueueFamilies(intersection, constructors[i]->_minRequested);
						}
						if (constructors[j]->_queueFamilyIndices.size() < constructors[j]->_minRequested) {
							constructors[j]->expandWithQueueFamilies(intersection, constructors[j]->_minRequested);
						}
					}
					//TODO: Remove inserted values???
				}
			}
		}

		//Check that queue distribution satisfy minimum requirements
		for (const auto* constructor : constructors) {
			if (constructor->_queueFamilyIndices.size() < constructor->_minRequested) {
				return false;
			}
		}

		//Try to fill constructors to statisfy maximum values
		for (int i = 0; i < constructors.size(); i++) {
			for (int j = i + 1; j < constructors.size(); j++) {
				//if intersection was found
				if (intersections.count(ConstructorIndicesPair(i, j))) {
					auto& intersection = intersections.at(ConstructorIndicesPair(i, j));
					if (constructors[i]->canBeSame(*constructors[j])) {
						if (constructors[i]->_queueFamilyIndices.size() < constructors[i]->_maxRequested) {
							constructors[i]->expandWithQueueFamilies(intersection, constructors[i]->_maxRequested);
						}
						if (constructors[j]->_queueFamilyIndices.size() < constructors[j]->_minRequested) {
							constructors[j]->expandWithQueueFamilies(intersection, constructors[j]->_maxRequested);
						}
					}
					//TODO: Remove inserted values???
				}
			}
		}

		return true;
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