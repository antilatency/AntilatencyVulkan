#pragma once
#include <vector>
#include <set>
#include <algorithm>

#include "AntilatencyVulkanCommon.h"
#include "LibraryLoader.h"
#include "Functions/VulkanFunction.h"
#include "Functions/VulkanFunctionGroup.h"
#include "VulkanInstanceFactory.h"

/*
#include "Functions/Instance/vkGetInstanceProcAddr.h"
#include "Functions/Instance/vkCreateInstance.h"
#include "Functions/Instance/vkEnumerateInstanceLayerProperties.h"
#include "Functions/Instance/vkEnumerateInstanceExtensionProperties.h"*/

#include "VulkanInstance.h"

VulkanInstanceFunction(vkCreateInstance) };
VulkanInstanceFunction(vkEnumerateInstanceLayerProperties) };
VulkanInstanceFunction(vkEnumerateInstanceExtensionProperties) };

using VulkanInstanceFactoryFunctions = VulkanFunctionGroup<
	vkCreateInstance,
	vkEnumerateInstanceLayerProperties,
	vkEnumerateInstanceExtensionProperties
>;

template<class SetType>
void merge_sets(SetType& first, const SetType& last) {
    first.insert(last.begin(), last.end());
//    std::transform(last.begin(), last.end(), std::inserter(first),
//        [](const auto& c_str) {
//            return std::string(c_str);
//        });
};


//Forward declaration
class VulkanInstanceFactory;


class VulkanInstanceBuilder
{
public:
	VulkanInstanceBuilder& enableStandartDebug() {
		_enabledLayers.insert("VK_LAYER_LUNARG_standard_validation");
		return *this;
	}

	template<class ...T>
	VulkanInstanceBuilder& enableExtensions() {
        //_enabledExtensions.insert(std::string(T::getInstanceExtensionName()...));
        //_enabledExtensions.insert(std::string(T::getInstanceExtensionName()...));
        merge_sets(_enabledExtensions, T::getInstanceExtensionName()...);
		return *this;
	}

	Ref<VulkanInstance> createInstance();

	~VulkanInstanceBuilder() = default;

private:
	VulkanInstanceBuilder(VulkanInstanceFactory& factory)
		: _factory(factory)
	{}

	VulkanInstanceBuilder(const VulkanInstanceBuilder& other) :
		_factory(other._factory)
	{
		_enabledLayers = other._enabledLayers;
		_enabledExtensions = other._enabledExtensions;
	}
private:
	VulkanInstanceFactory& _factory;
	std::set<std::string> _enabledLayers;
	std::set<std::string> _enabledExtensions;

private:
	friend class VulkanInstanceFactory;
};



class VulkanInstanceFactory : public RefCounter {
private:
	Library library;
	PFN_vkGetInstanceProcAddr loaderFunction;

    VulkanInstanceFactoryFunctions functions;
	
	VulkanInstanceFactory() {

#if defined(_WIN32)
        const char* libraryName = "vulkan-1";
#elif defined(__ANDROID__) || defined(__linux__)
        const char* libraryName = "libvulkan";
#endif
        library = Library(libraryName);
		loaderFunction = (PFN_vkGetInstanceProcAddr)library.getFunction("vkGetInstanceProcAddr");
		functions.load(loaderFunction);
	}

	//Only Ref<VulkanInstanceFactory> can destroy VulkanInstanceFactory
	friend class Ref<VulkanInstanceFactory>; 
	~VulkanInstanceFactory() override {}

public:
	static auto create() {
		return Ref<VulkanInstanceFactory>(new VulkanInstanceFactory());
	}

	VulkanInstanceBuilder beginInstanceCreation() {
		return VulkanInstanceBuilder(*this);
	}

	std::vector<VkLayerProperties> enumerateLayerProperties() {
		return vulkanEnumerate(functions.get<vkEnumerateInstanceLayerProperties>().function);
	}

	std::vector<VkExtensionProperties> enumerateExtensionProperties() {
		return vulkanEnumerate(functions.get<vkEnumerateInstanceExtensionProperties>().function, nullptr);
	}

    bool isExtensionsSupported(std::vector<const char*> extensions) {
        auto supportedExtensions = enumerateExtensionProperties();

        //We do not care much about speed here

        for(const auto requestedExtension : extensions) {
            bool extensionSupported = false;
            for(const auto& supportedExtension : supportedExtensions) {
                if( strcmp(supportedExtension.extensionName, requestedExtension) == 0 ) {
                    extensionSupported = true;
                    break;
                }
            }

            if(extensionSupported == false) {
                return false;
            }
        }

        return true;
    }
	

	static const VkInstanceCreateInfo getDefaultCreateInfo() {
		VkInstanceCreateInfo result;
		result.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		result.pNext = nullptr;
		result.flags = 0;
		result.pApplicationInfo = nullptr;
		result.enabledLayerCount = 0;
		result.ppEnabledLayerNames = nullptr;
		result.enabledExtensionCount = 0;
		result.ppEnabledExtensionNames = nullptr;
		return result;
	}

	static const VkInstanceCreateInfo getDebugCreateInfo() {
		VkInstanceCreateInfo result = getDefaultCreateInfo();
		static const char* layerNames[1] = { "VK_LAYER_LUNARG_standard_validation" };
		result.enabledLayerCount = 1;
		result.ppEnabledLayerNames = layerNames;
		return result;
	}

    auto createInstance(VkInstanceCreateInfo createInfo, const std::vector<const char*>& extensions = {}, const VkAllocationCallbacks* pAllocator = nullptr) {
        //TODO: User may already specify extensions in createInfo. Expand them usin extensions array
        if (extensions.size() != 0) {
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();
        }

		VkInstance instance;
		functions.get<vkCreateInstance>().function(&createInfo, pAllocator, &instance);

		VulkanInstanceFunctions instanceFunctions;
		instanceFunctions.load(loaderFunction, instance);

		VulkanPhysicalDeviceFunctions physicalDeviceFunctions;
		physicalDeviceFunctions.load(loaderFunction, instance);

		return Ref<VulkanInstance>(new VulkanInstance(AbstractRef(this), instance, instanceFunctions, physicalDeviceFunctions, loaderFunction));

		//return VulkanInstance<VulkanInstanceFactory>::create(Ref<VulkanInstanceFactory>(this), instance);

	}
	
};

using VulkanInstanceFactoryRef = Ref<VulkanInstanceFactory>;




inline Ref<VulkanInstance> VulkanInstanceBuilder::createInstance() {
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	std::vector<const char*> extensions;
	std::transform(_enabledExtensions.begin(), _enabledExtensions.end(), std::back_inserter(extensions), 
		[](const auto& str) {
			return str.c_str();
		});

    extensions.push_back("VK_KHR_win32_surface");
	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();

	std::vector<const char*> layers;
	std::transform(_enabledLayers.begin(), _enabledLayers.end(), std::back_inserter(layers), 
		[](const auto& str) {
			return str.c_str();
		});

	createInfo.enabledLayerCount = layers.size();
	createInfo.ppEnabledLayerNames = layers.data();

    return _factory.createInstance(createInfo);
}
