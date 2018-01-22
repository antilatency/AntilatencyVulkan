#pragma once
#include <memory>

#include "VulkanInstanceFactory.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"

VulkanInstanceFunction(vkDestroyInstance) };
VulkanInstanceFunction(vkEnumeratePhysicalDevices) };
VulkanInstanceFunction(vkCreateWin32SurfaceKHR) };

using VulkanInstanceFunctions = VulkanFunctionGroup<
    vkDestroyInstance,
    vkEnumeratePhysicalDevices,

    vkCreateWin32SurfaceKHR
>;


class VulkanInstance : public RefCounter{
    friend class Ref<VulkanInstance>;
public:
    VkInstance instance = VK_NULL_HANDLE;
private:
    //using
    const AbstractRef factory;
    VulkanInstanceFunctions instanceFunctions;

    VulkanPhysicalDeviceFunctions physicalDeviceFunctions;

    std::vector<VulkanPhysicalDevice> physicalDevices;

    PFN_vkGetInstanceProcAddr loaderFunction;

public:

    VulkanInstance(
        const AbstractRef& factory,
        VkInstance instance,
        const VulkanInstanceFunctions& instanceFunctions,
        const VulkanPhysicalDeviceFunctions& physicalDeviceFunctions,
        PFN_vkGetInstanceProcAddr loaderFunc
    ) :
        factory(factory),
        instance(instance),
        instanceFunctions(instanceFunctions),
        physicalDeviceFunctions(physicalDeviceFunctions),
        loaderFunction(loaderFunc)
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

    template<typename T, typename... Args>
    Ref<T> get(Args... args);

#ifdef VK_USE_PLATFORM_WIN32_KHR
    template<>
    Ref<VulkanSurface> get<VulkanSurface>(HINSTANCE winInstance, HWND hwnd) {
        VkWin32SurfaceCreateInfoKHR createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hinstance = winInstance;
        createInfo.hwnd = hwnd;

        VkSurfaceKHR surface;
        auto createSurfaceFunc = instanceFunctions.get<vkCreateWin32SurfaceKHR>().function;
        createSurfaceFunc(instance, &createInfo, nullptr, &surface);

        VulkanSurfaceFunctions surfaceFunctions;
        surfaceFunctions.load(loaderFunction, instance);

        return Ref<VulkanSurface>( new VulkanSurface(Ref<VulkanInstance>(this), surface, surfaceFunctions) );
    }
#endif

};

//using VulkanInstanceRef = Ref<VulkanInstance>;
