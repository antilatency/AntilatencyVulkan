#pragma once

#include "Builders/SwapChainImageViewBuilder.h"

#include "VulkanSwapChain.h"
#include "VulkanImageView.h"

auto SwapChainImageViewBuilder::createImageView() {
    _createInfo.format = _swapChainRef->getImageFormat();
    _createInfo.image = _imageRef->getHandle();

    auto deviceHandle = _swapChainRef->getDevice()->getHandle();

    VkImageView _imageViewHandle;

    auto res = _swapChainRef->getDevice()->invokeVulkanFunction<vkCreateImageView>( &_createInfo, nullptr, &_imageViewHandle );
    assert( res == VK_SUCCESS );

    return VulkanImageView::create( _imageViewHandle, _swapChainRef->getDevice(), _imageRef );
}