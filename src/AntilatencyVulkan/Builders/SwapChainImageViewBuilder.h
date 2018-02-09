#pragma once

#include <vector>

#include "AntilatencyVulkanCommon.h"

class VulkanSwapChain;
class VulkanImage;


class SwapChainImageViewBuilder {
    friend class VulkanSwapChain;

    using ImageIdentifier = uint32_t;
    using CreateInfosContainer = std::vector< std::pair<ImageIdentifier, VkImageViewCreateInfo> >;

public:
    auto& setComponentsMapping( const VkComponentMapping& componentsMapping ) {
        _createInfo.components = componentsMapping;
        return *this;
    }


    auto& setComponentMappingR( const VkComponentSwizzle r ) {
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


    auto& setSubresourceRangeLayerCount( const uint32_t layerCount ) {
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
    SwapChainImageViewBuilder( const Ref<VulkanSwapChain>& swapChainRef, const Ref<VulkanImage>& imageRef ) :
        _swapChainRef( swapChainRef ),
        _imageRef( imageRef ) {};

private:
    Ref<VulkanSwapChain> _swapChainRef;
    Ref<VulkanImage> _imageRef;
    VkImageViewCreateInfo _createInfo = getDefaultCreateInfo();
};