#pragma once

#include "AntilatencyVulkanCommon.h"

#include "VulkanDevice.h"

//There is a two versions of images -> who controls its lifetime and who not. 
//The first case example is allocated image on device manually by user.
//The second case example is internal swapchain images whose lifetime is controlled by swapchain itself.

class VulkanImage : public RefCounter {
    friend class Ref<VulkanImage>;
public:
    VulkanImage(VkImage imageHandle) :
        _imageHandle(imageHandle)
    {}

    auto getHandle() const {
        return _imageHandle;
    }

protected:
    VkImage _imageHandle;
};


class vkDestroyImage;


class DeviceImage final : public VulkanImage {
    friend class Ref<DeviceImage>;
public:
    static auto create( const VkImage imageHandle, const Ref<VulkanDevice>& deviceRef ) {
        return Ref<VulkanImage>( new DeviceImage( imageHandle, deviceRef ) );
    }

    ~DeviceImage() {
        _deviceRef->invokeVulkanFunction<vkDestroyImage>(_imageHandle, nullptr );
    }

private:
    DeviceImage( const VkImage imageHandle, const Ref<VulkanDevice>& deviceRef ) :
        VulkanImage(imageHandle),
        _deviceRef(deviceRef)
    {}

private:
    Ref<VulkanDevice> _deviceRef;
};


class VulkanSwapChain;


class SwapChainImage final : public VulkanImage {
    friend class Ref<SwapChainImage>;
public:
    static auto create( const VkImage imageHandle, const Ref<VulkanSwapChain>& swapChainRef ) {
        return Ref<VulkanImage>( new SwapChainImage(imageHandle, swapChainRef) );
    }

private:
    SwapChainImage( const VkImage imageHandle, const Ref<VulkanSwapChain>& swapChainRef ) :
        VulkanImage( imageHandle ),
        _swapChainRef( swapChainRef )
    {}

private:
    Ref<VulkanSwapChain> _swapChainRef;
};
