#pragma once

#include <algorithm>

#include "AntilatencyVulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"

#include "Builders/SwapChainImageViewBuilder.h"


VulkanDeviceFunction(vkCreateSwapchainKHR) };
VulkanDeviceFunction(vkDestroySwapchainKHR) };
VulkanDeviceFunction(vkGetSwapchainImagesKHR) };
VulkanDeviceFunction(vkAcquireNextImageKHR) };
VulkanDeviceFunction(vkQueuePresentKHR) };


using SwapChainFunctions = VulkanFunctionGroup<
	vkCreateSwapchainKHR,
	vkDestroySwapchainKHR,
	vkGetSwapchainImagesKHR,
	vkAcquireNextImageKHR,
	vkQueuePresentKHR
>;


class VulkanSwapChain : public RefCounter {
	friend class Ref<VulkanSwapChain>;

public:
	static auto create(VkSwapchainKHR swapchainHandle, const VkFormat imageFormat, const Ref<VulkanDevice>& deviceRef, SwapChainFunctions* functions) {
        assert( functions->isAllFunctionsLoaded() && "Not all SwapChain functions is loaded");
        assert( swapchainHandle != VK_NULL_HANDLE );

        //FIXME: Move to another function
        auto getSwapChainImagesFunc = functions->get<vkGetSwapchainImagesKHR>().function;
        
        uint32_t imagesCount;
        auto res = getSwapChainImagesFunc( deviceRef->getHandle(), swapchainHandle, &imagesCount, nullptr );
        assert( res == VK_SUCCESS );

        std::vector<VkImage> imagesHandles(imagesCount);
        res = getSwapChainImagesFunc( deviceRef->getHandle(), swapchainHandle, &imagesCount, imagesHandles.data() );
        assert( res == VK_SUCCESS );

		return Ref<VulkanSwapChain>(new VulkanSwapChain(swapchainHandle, imageFormat, imagesHandles,  deviceRef, functions));
	}

    auto beginImageViewCreation( const Ref<VulkanImage>& imageRef ) {
        return SwapChainImageViewBuilder( Ref<VulkanSwapChain>( this ), imageRef );
    }

    auto getHandle() const {
        return _swapchainHandle;
    }

    auto getDevice() const {
        return _deviceRef;
    }

    auto getImageFormat() const {
        return _imageFormat;
    }

    auto getImages() {
        std::vector<Ref<VulkanImage>> out;
        for(const auto& imageHandle : _imagesHandles) {
            out.push_back(SwapChainImage::create( imageHandle, Ref<VulkanSwapChain>( this ) ));
        }
        return out;
    }

private:
	VulkanSwapChain(const VkSwapchainKHR swapchainHandle, const VkFormat imageFormat, const std::vector<VkImage>& imagesHandles, const Ref<VulkanDevice>& deviceRef,  SwapChainFunctions* functions) :
		_swapchainHandle(swapchainHandle),
        _imagesHandles(imagesHandles),
		_deviceRef(deviceRef),
		_functions(functions),
        _imageFormat(imageFormat)
	{}

private:
	VkSwapchainKHR _swapchainHandle;
    std::vector<VkImage> _imagesHandles;
	
	//We can not use SwapChainExtension in that place, because SwapChain and SwapChainExtension references each other in include hierarchy.
	Ref<VulkanDevice> _deviceRef;
	SwapChainFunctions* _functions;
    VkFormat _imageFormat;
};
