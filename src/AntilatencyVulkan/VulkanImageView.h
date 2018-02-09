#pragma once

#include "AntilatencyVulkanCommon.h"


class VulkanImage;
class VulkanSwapChain;

class vkDestroyImageView;


class VulkanImageView : public RefCounter {
    friend class Ref<VulkanImageView>;
public:
    static auto create( const VkImageView imageViewHandle, const Ref<VulkanDevice> deviceRef, const Ref<VulkanImage>& imageRef ) {
        return Ref<VulkanImageView>( new VulkanImageView( imageViewHandle, deviceRef, imageRef ) );
    }
    
    auto getHandle() {
        return _imageViewHandle;
    }

    ~VulkanImageView() {
        _deviceRef->invokeVulkanFunction<vkDestroyImageView>( _imageViewHandle, nullptr );
    }

protected:
    //In that case we control image lifetime
    VulkanImageView(const VkImageView imageViewHandle, const Ref<VulkanDevice> deviceRef, const Ref<VulkanImage> imageRef ) :
        _imageViewHandle( imageViewHandle ),
        _deviceRef( deviceRef ),
        _imageRef(imageRef)
    {}

protected:
    VkImageView _imageViewHandle;
    Ref<VulkanImage> _imageRef;
    Ref<VulkanDevice> _deviceRef;
};

//FIXME: Does we need to implement different versions of VulkanImageView like we did with VulkanImage?