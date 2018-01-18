CONFIG += c++14

INCLUDEPATH += $(VULKAN_SDK)/Include/vulkan

INCLUDEPATH += $$PWD/AntilatencyVulkan

HEADERS += $$PWD/AntilatencyVulkan/AntilatencyVulkanCommon.h \
           $$PWD/AntilatencyVulkan/LibraryLoader.h \
           $$PWD/AntilatencyVulkan/VulkanInstance.h \
           $$PWD/AntilatencyVulkan/VulkanInstanceFactory.h \
           $$PWD/AntilatencyVulkan/VulkanPhysicalDevice.h \
           $$PWD/AntilatencyVulkan/VulkanPhysicalDeviceFunctions.h

HEADERS += $$PWD/AntilatencyVulkan/Functions/VulkanFunction.h \
           $$PWD/AntilatencyVulkan/Functions/VulkanFunctionGroup.h
