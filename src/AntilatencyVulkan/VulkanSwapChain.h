#pragma once

#include <algorithm>

#include "AntilatencyVulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanImageView.h"


class VulkanSwapChain;


class SwapChainImageViewBuilder {
    friend class VulkanSwapChain;

    using ImageIdentifier = uint32_t;
    using CreateInfosContainer = std::vector< std::pair<ImageIdentifier, VkImageViewCreateInfo> >;

public:
    auto& setComponentsMapping( const VkComponentMapping& componentsMapping ) {
       _createInfo.components = componentsMapping;
        return *this;
    }


    auto& setComponentMappingR( const VkComponentSwizzle r) {
        _createInfo.components.r = r;
        return *this;
    }


    auto& setComponentMappingG( const VkComponentSwizzle g ) {
        _createInfo.components.g = g;
        return *this;
    }


    auto& setComponentMappingB( const VkComponentSwizzle b ) {
        _createInfo.components.b = b;
        return *this;
    }


    auto& setComponentMappingA( const VkComponentSwizzle a ) {
        _createInfo.components.a = a;
        return *this;
    }


    auto& setSubresourceRangeAspectMask( const VkImageAspectFlagBits aspectMask ) {
        _createInfo.subresourceRange.aspectMask = aspectMask;
        return *this;
    }


    auto& setSubresourceRangeBaseMipLevel( const uint32_t baseMipLevel ) {
        _createInfo.subresourceRange.baseMipLevel = baseMipLevel;
        return *this;
    }


    auto& setSubresourceRangeLevelCount( const uint32_t levelCount ) {
        _createInfo.subresourceRange.levelCount = levelCount;
        return *this;
    }


    auto& setSubresourceRangeBaseArrayLayer( const uint32_t baseArrayLayer ) {
        _createInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
        return *this;
    }


    auto& setSubresourceRangeLayerCount(const uint32_t layerCount ) {
        _createInfo.subresourceRange.layerCount = layerCount;
        return *this;
    }


    static VkImageViewCreateInfo getDefaultCreateInfo() {
        VkImageViewCreateInfo createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

        createInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY,
                                  VK_COMPONENT_SWIZZLE_IDENTITY,
                                  VK_COMPONENT_SWIZZLE_IDENTITY,
                                  VK_COMPONENT_SWIZZLE_IDENTITY };

        createInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        return createInfo;
    }


    auto createImageView();

private:
    SwapChainImageViewBuilder(const Ref<VulkanSwapChain>& swapChainRef, const Ref<VulkanImage>& imageRef) :
        _swapChainRef(swapChainRef),
        _imageRef(imageRef)
    {};

private:
    Ref<VulkanSwapChain> _swapChainRef;
    Ref<VulkanImage> _imageRef;
    VkImageViewCreateInfo _createInfo = getDefaultCreateInfo();
};


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



auto SwapChainImageViewBuilder::createImageView()
{
    _createInfo.format = _swapChainRef->getImageFormat();
    _createInfo.image = _imageRef->getHandle();

    auto deviceHandle = _swapChainRef->getDevice()->getHandle();

    VkImageView _imageViewHandle;

    auto res = _swapChainRef->getDevice()->invokeVulkanFunction<vkCreateImageView>( &_createInfo, nullptr, &_imageViewHandle );
    assert( res == VK_SUCCESS );

    return VulkanImageView::create( _imageViewHandle, _swapChainRef->getDevice(), _imageRef);
}
