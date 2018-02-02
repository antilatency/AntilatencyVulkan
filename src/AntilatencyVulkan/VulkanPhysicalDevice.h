#pragma once

#include "AntilatencyVulkanCommon.h"
#include "Functions/VulkanFunction.h"
#include "Functions/VulkanFunctionGroup.h"
#include "QueueConstructor.h"
#include "QueueGarden.h"
#include "VulkanDevice.h"


VulkanInstanceFunction(vkGetPhysicalDeviceFeatures) };
VulkanInstanceFunction(vkGetPhysicalDeviceFormatProperties) };
VulkanInstanceFunction(vkGetPhysicalDeviceImageFormatProperties) };
VulkanInstanceFunction(vkGetPhysicalDeviceProperties) };
VulkanInstanceFunction(vkGetPhysicalDeviceQueueFamilyProperties) };
VulkanInstanceFunction(vkGetPhysicalDeviceMemoryProperties) };
VulkanInstanceFunction(vkCreateDevice) };
VulkanInstanceFunction(vkEnumerateDeviceExtensionProperties) };
VulkanInstanceFunction(vkEnumerateDeviceLayerProperties) };

//This functions is belong to extensions
//VulkanInstanceFunction(vkGetPhysicalDeviceDisplayPropertiesKHR) };
//VulkanInstanceFunction(vkGetDisplayPlaneCapabilitiesKHR) };

VulkanInstanceFunction(vkGetDeviceProcAddr) };

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

	vkGetDeviceProcAddr
>;


class VulkanInstance;
class VulkanPhysicalDevice;

template<typename... Extensions>
class VulkanDeviceBuilder {
	template<typename... Args>
	friend class VulkanDeviceBuilder;

	friend class VulkanPhysicalDevice;
public:
	template<class ...T>
	auto enableExtensions() {
		using TupleType = std::tuple<T...>;
		constexpr auto extensionsCount = std::tuple_size<TupleType>::value;

		enableExtensions_impl<TupleType, extensionsCount - 1>();
		return VulkanDeviceBuilder<T...>(_physicalDevice, _enabledExtensions, _constructors);
	}

	auto& declareQueues(const std::vector<QueueConstructor*>& constructors) {
		_constructors = constructors;
		return *this;
	}

	auto& declareFeatures() {
		static_assert(false, "Not implemented");
		return *this;
	}

	auto createDevice();

private:
	VulkanDeviceBuilder(const Ref<VulkanPhysicalDevice>& physicalDevice)
		: _physicalDevice(physicalDevice)
	{}

	VulkanDeviceBuilder(const Ref<VulkanPhysicalDevice>& physicalDevice, const std::set<std::string>& enabledExtensions, const std::vector<QueueConstructor*>& constructors) :
		_physicalDevice(physicalDevice),
		_enabledExtensions(enabledExtensions),
		_constructors(constructors)
	{}

	/*VulkanDeviceBuilder(const VulkanDeviceBuilder& other) :
		_factory(other._factory)
	{
		_enabledLayers = other._enabledLayers;
		_enabledExtensions = other._enabledExtensions;
	}*/

	template<class TupleType, int32_t I >
	auto enableExtensions_impl() {
		if constexpr(I > 0) {
			using ExtensionType = typename std::tuple_element<I, TupleType>::type;

			merge_sets(_enabledExtensions, ExtensionType::requiredExtensionNamesStatic());
			enableExtensions_impl<TupleType, I - 1>();
		}
		else if constexpr (I == 0) {
			using ExtensionType = typename std::tuple_element<0, TupleType>::type;
			merge_sets(_enabledExtensions, ExtensionType::requiredExtensionNamesStatic());
		}
	}

private:
	Ref<VulkanPhysicalDevice> _physicalDevice;
	std::set<std::string> _enabledExtensions;
	std::vector<QueueConstructor*> _constructors;
};


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

	//TODO: Возвращать некий QueueDestribution с перегруженным оператором bool
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
		return VulkanDeviceBuilder<>(Ref<VulkanPhysicalDevice>(this));
		/*VkDevice device;
		auto function = functions->get<vkCreateDevice>().function;
		VkDeviceCreateInfo createInfo;
		createInfo.
		function(physicalDevice,)*/
	}

	template<class Extensions = NullType>
	auto createDevice(const VkDeviceCreateInfo& createInfo) {
		VkDevice device;

		auto createDeviceFunc = _functions->get<vkCreateDevice>().function;

		if (createDeviceFunc(_physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			return Ref<VulkanDevice>(nullptr);
		}

		auto loaderFunction = _functions->get<vkGetDeviceProcAddr>().function;
		_deviceFunctions.load(loaderFunction, device);

		assert(_deviceFunctions.isAllFunctionsLoaded());

		std::vector<std::string> enabledExtensions(createInfo.ppEnabledExtensionNames, createInfo.ppEnabledExtensionNames + createInfo.enabledExtensionCount);
		
		std::vector< std::pair<uint32_t, uint32_t> > queues;

		/*for (int i = 0; i < createInfo.queueCreateInfoCount; i++) {
			const auto& queueCreateInfo = createInfo.pQueueCreateInfos[i];
			queues.push_back(std::make_pair(queueCreateInfo->queueFamilyIndex, queueCreateInfo->queueCount));
		}*/

		return VulkanDevice::create<Extensions>(Ref<VulkanPhysicalDevice>(this), device, enabledExtensions, _deviceFunctions);
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
	VulkanDeviceFunctions _deviceFunctions;
};




template<typename... Extensions>
auto VulkanDeviceBuilder<Extensions...>::createDevice() {
	//Create queues Create Infos
	using QueueFamily = uint32_t;
	using QueueCount = uint32_t;

	std::map<QueueFamily, QueueCount> queuesForCreating;

	//In each queue constructor we have declaration of which queue in which family we must to use

	//The indexes are distributed in such a way that the maximum index of the queue for family for all constructors
	//corresponds to the required number of queues - 1 in that family.

	//Thus traverse all constructors, for each queue family find maximum index of queue used for that family,
	//request (maximum_index_of_queue + 1) queues for corresponding family.
	for (const auto constructor : _constructors) {
		for (auto&& [familyIndex, queueIndex] : constructor->_queueIndexes) {
			if (queuesForCreating.count(familyIndex) == 0) {
				queuesForCreating.insert(std::make_pair(familyIndex, queueIndex));
			}
			else {
				const auto& recordedValues = queuesForCreating[familyIndex];
				if (queueIndex > recordedValues) {
					queuesForCreating[familyIndex] = queueIndex;
				}
			}
		}
	}

	std::vector<VkDeviceQueueCreateInfo> queueInfos;

	//TODO: priority;
	float queuePriority = 1.0f;
	for (const auto&[familyIndex, queueCount] : queuesForCreating) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = familyIndex;
		queueCreateInfo.queueCount = queueCount + 1;

		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueInfos.push_back(std::move(queueCreateInfo));
	}

	//TODO: Device Features.
	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	//fill queues field
	deviceCreateInfo.pQueueCreateInfos = queueInfos.data();
	deviceCreateInfo.queueCreateInfoCount = queueInfos.size();

	//Fill features field
	deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

	//Fill extensions field
	std::vector<const char*> extensions;
	std::transform(_enabledExtensions.begin(), _enabledExtensions.end(), std::back_inserter(extensions),
		[](const auto& str) {
		return str.c_str();
	});
	deviceCreateInfo.enabledExtensionCount = extensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = extensions.data();


	//FIXME: declaration of validation layers is deprecated. See Vulkan specifications. Does we need to grab declaration from instance and put them anyway?
	deviceCreateInfo.enabledLayerCount = 0;

	//deviceCreateInfo.enabledLayerCount = _physicalDevice->instance->validationLayers.size();
	//deviceCreateInfo.ppEnabledLayerNames = _physicalDevice->instance->validationLayers.data();

	using TupleArgs = std::tuple<Extensions...>;
	constexpr auto extensionsCount = std::tuple_size<TupleArgs>::value;

	if constexpr(extensionsCount > 0) {
		auto vkDevice = _physicalDevice->createDevice<TypeList<Extensions...>>(deviceCreateInfo);
		return vkDevice;
	}
	else {
		auto vkDevice = _physicalDevice->createDevice<NullType>(deviceCreateInfo);
		return vkDevice;
	}

	//TODO: Retrieve queue handlers and fill constructors
};
