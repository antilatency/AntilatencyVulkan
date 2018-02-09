#pragma once

#include <array>
#include <limits>

#include "AntilatencyVulkanCommon.h"
#include "Utils/CreateInfoUtils.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"

#include "DeviceExtensions/DeviceExtension.h"


class SwapChainDeviceExtension;


class SwapChainBuilder {
public:
    using QueueFamilyIndex = uint32_t;

    SwapChainBuilder(const Ref<SwapChainDeviceExtension>& swapchainExtensionRef,
                     const Ref<VulkanDevice>& deviceRef,
                     const Ref<VulkanSurface>& surfaceRef) :
        _swapchainExtensionRef(swapchainExtensionRef),
        _deviceRef(deviceRef),
		_surfaceRef(surfaceRef)
	{}

    auto createSwapChain();

	auto& setClipped(bool enabled) {
        _clipped = enabled;
		return *this;
	}

	auto& setImageArrayLayers(uint32_t imageArrayLayers) {
		_imageArrayLayers = imageArrayLayers;
		return *this;
	}

	auto& setImageUsage(VkImageUsageFlags imageUsage) {
		_imageUsage = imageUsage;
		return *this;
	}

	auto& setAllowedQueues(const std::vector<QueueFamilyIndex>& indices) {
		_allowedQueuesFamilies = indices;
		return *this;
	}

	//TODO :: replaceSwapchain????
	auto& setOldSwapchain(VkSwapchainKHR oldSwapchain) {
		_oldSwapchain = oldSwapchain;
		return *this;
	}

	auto& setTransform(VkSurfaceTransformFlagBitsKHR preTransform) {
		_preTransform = preTransform;
		return *this;
	}

	//---Present mode-----

	auto& requiredPresentMode(VkPresentModeKHR presentMode) {
		_presentMode = CreateInfoOption<VkPresentModeKHR>(presentMode, false);
		return *this;
	}

	auto& requiredOneOfPresentModes(std::initializer_list<VkPresentModeKHR> presentModes) {
		_presentMode = CreateInfoOption<VkPresentModeKHR>(presentModes, false);
		return *this;
	}

	auto& desiredPresentMode(VkPresentModeKHR presentMode) {
		_presentMode = CreateInfoOption<VkPresentModeKHR>(presentMode, true);
		return *this;
	}

	auto& desiredOneOfPresentModes(std::initializer_list<VkPresentModeKHR> presentModes) {
		_presentMode = CreateInfoOption<VkPresentModeKHR>(presentModes, true);
		return *this;
	}

    //---Surface Image format-----

    auto& requiredSurfaceFormat( VkFormat surfaceImageFormat )
    {
        _surfaceImageFormat = CreateInfoOption<VkFormat>( surfaceImageFormat, false );
        return *this;
    }

    auto& requiredOneOfSurfaceFormats( std::initializer_list<VkFormat> surfaceImageFormats )
    {
        _surfaceImageFormat = CreateInfoOption<VkFormat>( surfaceImageFormats, false );
        return *this;
    }

    auto& desiredSurfaceFormat( VkFormat surfaceImageFormat )
    {
        _surfaceImageFormat = CreateInfoOption<VkFormat>( surfaceImageFormat, true );
        return *this;
    }

    auto& desiredSurfaceFormats( std::initializer_list<VkFormat> surfaceImageFormats )
    {
        _surfaceImageFormat = CreateInfoOption<VkFormat>( surfaceImageFormats, true );
        return *this;
    }

	//---Color Space-----

	auto& requiredSurfaceColorSpace(VkColorSpaceKHR colorSpace) {
		_colorSpace = CreateInfoOption<VkColorSpaceKHR>( colorSpace, false);
		return *this;
	}

	auto& requiredOneOfSurfaceColorSpaces(std::initializer_list<VkColorSpaceKHR> colorSpaces ) {
        _colorSpace = CreateInfoOption<VkColorSpaceKHR>( colorSpaces, false );
		return *this;
	}

	auto& desiredSurfaceColorSpace(VkColorSpaceKHR colorSpace ) {
        _colorSpace = CreateInfoOption<VkColorSpaceKHR>( colorSpace, true );
		return *this;
	}

	auto& desiredSurfaceColorSpaces(std::initializer_list<VkColorSpaceKHR> colorSpaces ) {
        _colorSpace = CreateInfoOption<VkColorSpaceKHR>( colorSpaces, true );
		return *this;
	}

	//---Swap extent----

	auto& requiredSwapExtent(VkExtent2D swapExtent) {
		_swapExtent = CreateInfoOption<VkExtent2D>(swapExtent, false);
		return *this;
	}

	auto& requiredOneOfSwapExtent(std::initializer_list<VkExtent2D> swapExtents) {
		_swapExtent = CreateInfoOption<VkExtent2D>(swapExtents, false);
		return *this;
	}

	auto& desiredSwapExtent(VkExtent2D swapExtent) {
		_swapExtent = CreateInfoOption<VkExtent2D>(swapExtent, true);
		return *this;
	}

	auto& desiredOneOfSwapExtents(std::initializer_list<VkExtent2D> swapExtents) {
		_swapExtent = CreateInfoOption<VkExtent2D>(swapExtents, true);
		return *this;
	}

	//---Min image count----

	auto& requiredMinImageCount(uint32_t minImageCount) {
		_minImageCount = CreateInfoOption<uint32_t>(minImageCount, false);
		return *this;
	}

	auto& requiredOneOfMinImageCounts(std::initializer_list<uint32_t> minImagesCount) {
		_minImageCount = CreateInfoOption<uint32_t>(minImagesCount, false);
		return *this;
	}

	auto& desiredMinImageCount(uint32_t minImageCount) {
		_minImageCount = CreateInfoOption<uint32_t>(minImageCount, true);
		return *this;
	}

	auto& desiredOneOfMinImageCount(std::initializer_list<uint32_t> minImagesCount) {
		_minImageCount = CreateInfoOption<uint32_t>(minImagesCount, true);
		return *this;
	}

private:
    auto selectSwapExtent( const VkSurfaceCapabilitiesKHR & surfaceCapabilities ) const -> std::optional<VkExtent2D>;
    auto selectImageFormat( const std::vector<VkSurfaceFormatKHR>& surfaceFormats ) const->std::optional<VkFormat>;
    auto selectPresentMode( const std::vector<VkPresentModeKHR>& presentModes ) const -> std::optional<VkPresentModeKHR>;
    auto selectMinImageCount( const VkSurfaceCapabilitiesKHR& surfaceCapabilities ) const -> std::optional<uint32_t>;
    auto selectColorSpace( std::vector<VkSurfaceFormatKHR> surfaceFormats, VkFormat selectedImageFormat ) const -> std::optional<VkColorSpaceKHR>;


private:
	Ref<SwapChainDeviceExtension> _swapchainExtensionRef;
	Ref<VulkanDevice> _deviceRef;
	Ref<VulkanSurface> _surfaceRef;
	
	//Only setValue available
	VkSwapchainKHR _oldSwapchain = VK_NULL_HANDLE;
	bool _clipped = VK_FALSE;
	uint32_t _imageArrayLayers = 1;
	VkImageUsageFlags _imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VkSurfaceTransformFlagBitsKHR _preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	VkCompositeAlphaFlagBitsKHR _compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	std::vector<QueueFamilyIndex> _allowedQueuesFamilies;
		
	//Available declaring requirements/desires and using as a single value or as an ordered list
	CreateInfoOption<VkExtent2D> _swapExtent;
	CreateInfoOption<uint32_t> _minImageCount;
	CreateInfoOption<VkPresentModeKHR> _presentMode;
    CreateInfoOption<VkFormat> _surfaceImageFormat;
    CreateInfoOption<VkColorSpaceKHR> _colorSpace;

    //Defaults
    const std::array<VkPresentModeKHR, 6> DEFAULT_PRESENT_MODES = { VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR,
                                                                    VK_PRESENT_MODE_FIFO_RELAXED_KHR, VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR,  
                                                                    VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR };
    const VkColorSpaceKHR DEFAULT_COLOR_SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    const VkFormat DEFAULT_IMAGE_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

};


class SwapChainDeviceExtension : public DeviceExtension {
	friend class Ref<SwapChainDeviceExtension>;
	friend class SwapChainBuilder;

public:
    using FunctionGroupType = SwapChainFunctions;

	constexpr static auto requiredExtensionNamesStatic() {
        constexpr std::array<const char*, 1> extensions = { {VK_KHR_SWAPCHAIN_EXTENSION_NAME} };
		return extensions;
	}

	static auto canBeCreated(const std::vector<std::string>& extensions) {
		for (const auto& requiredExt : requiredExtensionNamesStatic()) {
			auto extensionFound = std::find(extensions.begin(), extensions.end(), std::string(requiredExt)) != extensions.end();

			if (extensionFound == false) {
				return false;
			}
		}
		return true;
	}

	//TODO: write compile time hashing
	static std::string extensionTypeIdStatic() {
		return "SwapChainDeviceExtension";
	}

public:
    static auto create(const Ref<VulkanDevice>& device, FunctionGroupType* functions) {
		return Ref<SwapChainDeviceExtension>(new SwapChainDeviceExtension(device, functions));
	}

	auto beginSwapchainCreation(const Ref<VulkanSurface>& surfaceRef) {
		return SwapChainBuilder(Ref<SwapChainDeviceExtension>(this), _deviceRef, surfaceRef);
	}

private:
	auto _createSwapChain(const VkSwapchainCreateInfoKHR& createInfo) {

		VkSwapchainKHR swapChainHandle;

		auto createSwapChainFunc = _functions->get<vkCreateSwapchainKHR>().function;

		if (createSwapChainFunc(_deviceRef->getHandle(), &createInfo, nullptr, &swapChainHandle) != VK_SUCCESS) {
			return Ref<VulkanSwapChain>(nullptr);
		}

		return VulkanSwapChain::create(swapChainHandle, createInfo.imageFormat, _deviceRef, _functions);
	}

private:
	SwapChainDeviceExtension(const Ref<VulkanDevice>& device, FunctionGroupType* functions) :
		DeviceExtension(device),
		_functions(functions)
	{}

private:
	FunctionGroupType * _functions;
};


auto SwapChainBuilder::createSwapChain()
{

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _surfaceRef->getHandle();

    //Only one possible value
    createInfo.oldSwapchain = _oldSwapchain;
    createInfo.clipped = _clipped;
    createInfo.imageArrayLayers = _imageArrayLayers;
    createInfo.imageUsage = _imageUsage;
    createInfo.preTransform = _preTransform;
    createInfo.compositeAlpha = _compositeAlpha;

    if ( _allowedQueuesFamilies.size() < 2 ) {
        createInfo.pQueueFamilyIndices = nullptr; // optional
        createInfo.queueFamilyIndexCount = 0; //optional
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    else {
        createInfo.pQueueFamilyIndices = _allowedQueuesFamilies.data();
        createInfo.queueFamilyIndexCount = _allowedQueuesFamilies.size();
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    }


    auto surfaceCapabilities = _surfaceRef->getPhysicalDeviceCapabilities( _deviceRef->getPhysicalDevice() );
    auto surfaceFormats = _surfaceRef->getPhysicalDeviceFormats( _deviceRef->getPhysicalDevice() );
    auto presentModes = _surfaceRef->getPhysicalDevicePresentModes( _deviceRef->getPhysicalDevice() );

    //User can declare its wishes about this variables.

    //He can decide to try setting the concrete value, 
    //provide ordered set of values and one of them  must be setted, 
    //or advice some variable or ordered list of variables. 
    //He also may skip declaration of the variable.

    //We must correctly handle all this possible cases.
        
    if ( auto presentMode = selectPresentMode( presentModes ) ) {
        createInfo.presentMode = presentMode.value();
    }
    else {
        return Ref<VulkanSwapChain>( nullptr );
    }
    
    if ( auto imageExtent = selectSwapExtent( surfaceCapabilities ) ) {
        createInfo.imageExtent = imageExtent.value();
    }
    else {
        return Ref<VulkanSwapChain>( nullptr );
    }
    
    if ( auto minImageCount = selectMinImageCount( surfaceCapabilities ) ) {
        createInfo.minImageCount = minImageCount.value();
    }
    else {
        return Ref<VulkanSwapChain>( nullptr );
    }
    
    if ( auto imageFormat = selectImageFormat( surfaceFormats ) ) {
        createInfo.imageFormat = imageFormat.value();
    }
    else {
        return Ref<VulkanSwapChain>( nullptr );
    }

    if ( auto colorSpace = selectColorSpace( surfaceFormats, createInfo.imageFormat ) ) {
        createInfo.imageColorSpace = colorSpace.value();
    }
    else {
        return Ref<VulkanSwapChain>( nullptr );
    }

    return _swapchainExtensionRef->_createSwapChain( createInfo );
}


auto SwapChainBuilder::selectSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities ) const -> std::optional<VkExtent2D>
{
    //if _swapExtent is not set
    if ( _swapExtent.isUnset() )
    {
        //On some platforms We MUST match the resolution of the window by setting the width and height in the currentExtent member
        if ( surfaceCapabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)() ) {
            return { surfaceCapabilities.currentExtent };
        }
        else {
            assert( false && "Not implemented. Need to set default swapExtent" );
            return {};
        }
    }
    else {
        //Again: On some platforms We MUST match the resolution of the window by setting the width and height in the currentExtent member
        if ( surfaceCapabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)() ) {
            if ( _swapExtent.isSoftAssignment() ) {
                return { surfaceCapabilities.currentExtent };
            }
            else {
                //On this platform manual setup of extent size is impossible
                auto requestedExtentMatchPlatform = _swapExtent.getFirstSatisfiedRequirements( { surfaceCapabilities.currentExtent },
                                                                                               []( const auto& a, const auto& b ) {
                                                                                                   return a.width == b.width && a.height == b.height;
                                                                                               } );
                if ( requestedExtentMatchPlatform ) {
                    return { requestedExtentMatchPlatform.value() };
                }
                else {
                    return {};
                }
            }
        }
        else {
            //On that platform manual setup is allowed
            auto extentFound = _swapExtent.getFirstSatisfiedRequirements( surfaceCapabilities.minImageExtent, surfaceCapabilities.maxImageExtent,
                                                                          []( const auto& value, const auto& minV, const auto& maxV ) {
                                                                                return  ( minV.width <= value.width )   && ( value.width <= maxV.width ) &&
                                                                                        ( minV.height <= value.height ) && ( value.height <= maxV.height );
                                                                          } );
            if ( extentFound ) {
                return extentFound;
            }
            else {
                if ( _swapExtent.isSoftAssignment() ) {
                    //Get first value and trim it to satisfy requirements
                    VkExtent2D selectedSwapExtent = {};

                    selectedSwapExtent.width = (std::max)( surfaceCapabilities.minImageExtent.width,
                        (std::min)( surfaceCapabilities.maxImageExtent.width, _swapExtent.getValues()[0].width ) );
                    selectedSwapExtent.height = (std::max)( surfaceCapabilities.minImageExtent.height,
                        (std::min)( surfaceCapabilities.maxImageExtent.height, _swapExtent.getValues()[0].height ) );
                    return { selectedSwapExtent };
                }
                else {
                    return {};
                }
            }
        }
    }
};


auto SwapChainBuilder::selectMinImageCount(const VkSurfaceCapabilitiesKHR& surfaceCapabilities) const -> std::optional<uint32_t>
{
    //choose image count
    //uint32_t selectedMinImageCount = 0;
    if ( _minImageCount.isUnset() ) {
        //TODO: if user does not assign any value to minImageCount
        return { surfaceCapabilities.minImageCount };
    }
    else {
        if ( surfaceCapabilities.maxImageCount == 0 ) {
            //Vulkan allow set us any images count we want
            assert( _minImageCount.getValuesCount() );

            return { _minImageCount.getValues()[0] };
        }
        else {
            if ( auto imageCount = _minImageCount.getFirstSatisfiedRequirements( surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount ) ) {
                return { imageCount.value() };
            }
            else if ( _minImageCount.isHardAssignment() ) {
                return {};
            }
            else {
                //ToDO: is that need so?
                //auto val = _minImageCount.getClosest(surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);
                assert( _minImageCount.getValuesCount() );
                return { ( std::max )( surfaceCapabilities.minImageCount, ( std::min )( surfaceCapabilities.maxImageCount, _minImageCount.getValues()[0] ) ) };
            }
        }
    }
};


auto SwapChainBuilder::selectPresentMode(const std::vector<VkPresentModeKHR>& presentModes) const -> std::optional<VkPresentModeKHR>
{
    //Vulkan supports restricted types of present modes.

    VkPresentModeKHR selectedPresentMode;

    if ( _presentMode.isUnset() ) {
        if ( presentModes.size() == 0 ) {
            //present mode must be set
            return {};
        }

        auto defaultPresentModeOption = CreateInfoOption<VkPresentModeKHR>( DEFAULT_PRESENT_MODES, true );

        if ( auto value = defaultPresentModeOption.getFirstSatisfiedRequirements( presentModes ) ) {
            return { *value };
        }
        else {
            return {};
        }
    }
    else {
        if ( auto mode = _presentMode.selectValue( presentModes, []( const auto& a, const auto& b ) { return a != b; } ) ) {
            return { mode.value() };
        }
        else {
            return {};
        }
    }
};


auto SwapChainBuilder::selectImageFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) const -> std::optional<VkFormat>
{
    if ( _surfaceImageFormat.isUnset() ) {
        //in that case we need to manually select DEFAULT format
        assert( false && "Please choose default image format" );

        if ( surfaceFormats.size() == 1 && surfaceFormats[ 0 ].format == VK_FORMAT_UNDEFINED ) {
            //FIXME: is Vulkan allow to us select format??? What behavior we need to choose
            return { DEFAULT_IMAGE_FORMAT };
        }
        else {
            //in that case we must select format from allowed
            auto suitableSurfaceFormat = std::find_if( surfaceFormats.begin(), surfaceFormats.end(),
                                                       [=]( const auto& surfaceFormat ) {
                                                           return surfaceFormat.format == DEFAULT_IMAGE_FORMAT;
                                                       });
            if ( suitableSurfaceFormat != surfaceFormats.end() ) {
                return { DEFAULT_IMAGE_FORMAT };
            }
            else {
                assert( false && "Default image format is unsupported" );
                return {};
            }
        }
    }
    else {
        if ( surfaceFormats.size() == 1 && surfaceFormats[ 0 ].format == VK_FORMAT_UNDEFINED ) {
            //in that case we need to manually select user-defined format
            assert( _surfaceImageFormat.getValuesCount() );
            return { _surfaceImageFormat.getValues()[0] };
        }
        else {
            std::optional<VkFormat> foundedFormat = {};

            for ( const auto& value : _surfaceImageFormat.getValues() ) {
                for ( const auto& availableValue : surfaceFormats ) {
                    if ( value == availableValue.format ) {
                        foundedFormat = value;
                    }
                }
            }

            if ( foundedFormat )
            {
                return foundedFormat;
            }
            else if ( _surfaceImageFormat.isHardAssignment() ) {
                return {};
            }
            else {
                //TODO: What we gonna do? Select default format or get first available?
                assert( false && "What we gonna do? Select default format or get first available?" );
                return {};
            }
        }
    }
};


auto SwapChainBuilder::selectColorSpace( std::vector<VkSurfaceFormatKHR> surfaceFormats, VkFormat selectedImageFormat) const -> std::optional<VkColorSpaceKHR>
{
    //Leave only those SURFACE (VkSurfaceFormat) formats who contains only selected IMAGE (VkFormat) format.
    surfaceFormats.erase( std::remove_if( surfaceFormats.begin(), surfaceFormats.end(),
                                          [&]( const auto& a ) { return a.format == selectedImageFormat; } ) );

    if ( _colorSpace.isUnset() ) {
        assert( false && "Please choose default color space" );

        //in that case we need to manually select DEFAULT format
        if ( surfaceFormats.size() == 1 && surfaceFormats[ 0 ].format == VK_FORMAT_UNDEFINED ) {
            assert( false && "Please choose default color space" );
            return { DEFAULT_COLOR_SPACE };
        }
        else {
            //in that case we must select format from allowed
            auto suitableSurfaceFormat = std::find_if( surfaceFormats.begin(), surfaceFormats.end(),
                                                       [=]( const auto& surfaceFormat ) {
                                                           return surfaceFormat.colorSpace == DEFAULT_COLOR_SPACE;
                                                       } );

            if ( suitableSurfaceFormat != surfaceFormats.end() ) {
                return { DEFAULT_COLOR_SPACE };
            }
            else {
                assert( false && "Default image format is unsupported" );
                return {};
            }
        }
    }
    else {
        if ( surfaceFormats.size() == 1 && surfaceFormats[ 0 ].format == VK_FORMAT_UNDEFINED ) {
            //in that case we need to manually select user-defined format
            assert( _colorSpace.getValuesCount() );
            return { _colorSpace.getValues()[0] };
        }
        else {
            std::optional<VkColorSpaceKHR> foundedColorSpace = {};

            for ( const auto& value : _colorSpace.getValues() ) {
                for ( const auto& availableValue : surfaceFormats ) {
                    if ( value == availableValue.colorSpace ) {
                        foundedColorSpace = value;
                    }
                }
            }

            if ( foundedColorSpace ) {
                return foundedColorSpace;
            }
            else if ( _colorSpace.isHardAssignment() ) {
                return {};
            }
            else {
                //TODO: What we gonna do? Select default color space or get first available?
                assert( false && "What we gonna do? Select default color space or get first available?" );
                return {};
            }
        }
    }
}