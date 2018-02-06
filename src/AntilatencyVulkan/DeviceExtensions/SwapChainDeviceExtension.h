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

	auto& setPresentMode(VkPresentModeKHR presentMode) {
		_presentMode = CreateInfoOption<VkPresentModeKHR>(presentMode, false);
		return *this;
	}

	auto& setPresentMode(std::initializer_list<VkPresentModeKHR> presentModes) {
		_presentMode = CreateInfoOption<VkPresentModeKHR>(presentModes, false);
		return *this;
	}

	auto& advicePresentMode(VkPresentModeKHR presentMode) {
		_presentMode = CreateInfoOption<VkPresentModeKHR>(presentMode, true);
		return *this;
	}

	auto& advicePresentMode(std::initializer_list<VkPresentModeKHR> presentModes) {
		_presentMode = CreateInfoOption<VkPresentModeKHR>(presentModes, true);
		return *this;
	}

	//---Surface format-----

	auto& setSurfaceFormat(VkSurfaceFormatKHR surfaceFormat) {
		_surfaceFormat = CreateInfoOption<VkSurfaceFormatKHR>(surfaceFormat, false);
		return *this;
	}

	auto& setSurfaceFormat(std::initializer_list<VkSurfaceFormatKHR> surfaceFormats) {
		_surfaceFormat = CreateInfoOption<VkSurfaceFormatKHR>(surfaceFormats, false);
		return *this;
	}

	auto& adviceSurfaceFormat(VkSurfaceFormatKHR surfaceFormat) {
		_surfaceFormat = CreateInfoOption<VkSurfaceFormatKHR>(surfaceFormat, true);
		return *this;
	}

	auto& adviceSurfaceFormat(std::initializer_list<VkSurfaceFormatKHR> surfaceFormats) {
		_surfaceFormat = CreateInfoOption<VkSurfaceFormatKHR>(surfaceFormats, true);
		return *this;
	}

	//---Swap extent----

	auto& adviceSwapExtent(VkExtent2D swapExtent) {
		_swapExtent = CreateInfoOption<VkExtent2D>(swapExtent, true);
		return *this;
	}

	//---Min image count----

	auto& setMinImageCount(uint32_t minImageCount) {
		_minImageCount = CreateInfoOption<uint32_t>(minImageCount, false);
		return *this;
	}

	auto& setMinImageCount(std::initializer_list<uint32_t> minImagesCount) {
		_minImageCount = CreateInfoOption<uint32_t>(minImagesCount, false);
		return *this;
	}

	auto& adviceMinImageCount(uint32_t minImageCount) {
		_minImageCount = CreateInfoOption<uint32_t>(minImageCount, true);
		return *this;
	}

	auto& adviceMinImageCount(std::initializer_list<uint32_t> minImagesCount) {
		_minImageCount = CreateInfoOption<uint32_t>(minImagesCount, true);
		return *this;
	}


private:
	Ref<SwapChainDeviceExtension> _swapchainExtensionRef;
	Ref<VulkanDevice> _deviceRef;
	Ref<VulkanSurface> _surfaceRef;
	
	//Only setValue available
	VkSwapchainKHR _oldSwapchain = VK_NULL_HANDLE; //+
	bool _clipped = VK_FALSE; //+
	uint32_t _imageArrayLayers = 1; //+
	VkImageUsageFlags _imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //+
	
	//Unique behavior
	//VkExtent2D _swapExtent = { (std::numeric_limits<uint32_t>::max)(), (std::numeric_limits<uint32_t>::max)() };
	CreateInfoOption<VkExtent2D> _swapExtent;

	VkSurfaceTransformFlagBitsKHR _preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	VkCompositeAlphaFlagBitsKHR _compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	std::vector<QueueFamilyIndex> _allowedQueuesFamilies;

	
	//Available set/advice and Value, Enumeration
	CreateInfoOption<uint32_t> _minImageCount;
	CreateInfoOption<VkPresentModeKHR> _presentMode;
	CreateInfoOption<VkSurfaceFormatKHR>  _surfaceFormat;

	const std::array<VkPresentModeKHR, 6> DEFAULT_PRESENT_MODES = { VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR, 
																	VK_PRESENT_MODE_FIFO_RELAXED_KHR, VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR,  VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR };
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

		return VulkanSwapChain::create(swapChainHandle, _deviceRef, _functions);
	}

private:
	SwapChainDeviceExtension(const Ref<VulkanDevice>& device, FunctionGroupType* functions) :
		DeviceExtension(device),
		_functions(functions)
	{}

private:
	FunctionGroupType * _functions;
};


auto SwapChainBuilder::createSwapChain() {

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

	if (_allowedQueuesFamilies.size() < 2) {
		createInfo.pQueueFamilyIndices = nullptr;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	else {
		createInfo.pQueueFamilyIndices = _allowedQueuesFamilies.data();
		createInfo.queueFamilyIndexCount = _allowedQueuesFamilies.size();
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	}

	
	auto surfaceCapabilities = _surfaceRef->getPhysicalDeviceCapabilities(_deviceRef->getPhysicalDevice());
	auto surfaceFormats = _surfaceRef->getPhysicalDeviceFormats(_deviceRef->getPhysicalDevice());
	auto presentModes = _surfaceRef->getPhysicalDevicePresentModes(_deviceRef->getPhysicalDevice());
	
	//User can declare its wishes about this variables.
	
	//He can decide to try setting the concrete value, 
	//provide ordered set of values and one of them  must be setted, 
	//or advice some variable or ordered list of variables. 
	//He also may skip declaration of the variable.

	//We must correctly handle all this possible cases.

	//1. Setup present mode
	//Vulkan supports restricted types of present modes.
	
	//If present mode is not set, choose one of the default hardcoded ordered list
	VkPresentModeKHR selectedPresentMode = {};
	if (_presentMode.isUnset()) {
		if (presentModes.size() == 0) {
			//present mode must be set
			return Ref<VulkanSwapChain>(nullptr);
		}
		_presentMode = CreateInfoOption<VkPresentModeKHR>(SwapChainBuilder::DEFAULT_PRESENT_MODES, true);
		if (auto value = _presentMode.getFirstStatisfyedRequrenments(presentModes)) {
			selectedPresentMode = *value;
		}
		else {
			return Ref<VulkanSwapChain>(nullptr);
		}
	}
	else {
		if (auto mode = _presentMode.selectValue(presentModes, [](const auto& a, const auto& b) { return a != b; })) {
			selectedPresentMode = mode.value();
		}
		else {
			return Ref<VulkanSwapChain>(nullptr);
		}
	}
	createInfo.presentMode = selectedPresentMode;

	//Choose swap extent
	VkExtent2D selectedSwapExtent = {};
	//if _swapExtent is not set
	if (_swapExtent.isUnset())
	{
		//On some platforms We MUST match the resolution of the window by setting the width and height in the currentExtent member
		if (surfaceCapabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
			selectedSwapExtent = surfaceCapabilities.currentExtent;
		}
		else {
			assert(false && "Not implemented. Need to set default swapExtent");
		}
	}
	else {
		//Again: On some platforms We MUST match the resolution of the window by setting the width and height in the currentExtent member
		if (surfaceCapabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
			if (_swapExtent.isSoft()) {
				selectedSwapExtent = surfaceCapabilities.currentExtent;
			}
			else {
				//On this platform manual setup of extent size is impossible
				return Ref<VulkanSwapChain>(nullptr);
			}
		}
		else {
			//On that platform manual setup is allowed
			if (_swapExtent.isSoft()) {
				selectedSwapExtent.width = (std::max)(surfaceCapabilities.minImageExtent.width,
					(std::min)(surfaceCapabilities.maxImageExtent.width, _swapExtent._values[0].width));
				selectedSwapExtent.height = (std::max)(surfaceCapabilities.minImageExtent.height,
					(std::min)(surfaceCapabilities.maxImageExtent.height, _swapExtent._values[0].height));
			}
			else {
				//if swapchain satisfy all requirenments
				if (surfaceCapabilities.minImageExtent.width <= _swapExtent._values[0].width &&  _swapExtent._values[0].width <= surfaceCapabilities.maxImageExtent.width &&
					surfaceCapabilities.minImageExtent.height <= _swapExtent._values[0].height &&  _swapExtent._values[0].height <= surfaceCapabilities.maxImageExtent.height)
				{
					selectedSwapExtent = _swapExtent._values[0];
				}
				else {
					return Ref<VulkanSwapChain>(nullptr);
				}
			}
		}
	}
	createInfo.imageExtent = selectedSwapExtent;


	//choose image count
	uint32_t selectedMinImageCount = 0;
	if (_minImageCount.isUnset()) {
		//TODO: if user does not assign any value to minImageCount
		selectedMinImageCount = surfaceCapabilities.minImageCount;
	}
	else {
		if (surfaceCapabilities.maxImageCount == 0) {
			//Vulkan allow set us any images count we want
			selectedMinImageCount = _minImageCount._values[0];
		}
		else {
			if (auto imageCount = _minImageCount.getFirstStatisfyedRequrenments(surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount)) {
				selectedMinImageCount = imageCount.value();
			}
			else if (_minImageCount.isHardAssignment()) {
				return Ref<VulkanSwapChain>(nullptr);
			}
			else {
				//ToDO: is that need so?
				//auto val = _minImageCount.getClosest(surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);
				selectedMinImageCount = (std::max)(surfaceCapabilities.minImageCount, (std::min)(surfaceCapabilities.maxImageCount, _minImageCount._values[0]));
			}
		}
	}
	createInfo.minImageCount = selectedMinImageCount;

	//choose image format
	VkSurfaceFormatKHR selectedFormat;
	if (_surfaceFormat.isUnset()) {
		//in that case we need to manually select DEFAULT format
		if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
			assert(false && "Please choose default image format");
			selectedFormat = { VK_FORMAT_R8G8B8A8_SRGB , VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}
		else {
			for (const auto& surfaceFormat : surfaceFormats) {
				if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM &&
					surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					selectedFormat = surfaceFormat;
					break;
				}
			}

			selectedFormat = surfaceFormats[0];
		}
	}
	else {
		if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
			//Try to setup our format
			selectedFormat = _surfaceFormat._values[0];
		}
		else {
			if (auto format = _surfaceFormat.getFirstStatisfyedRequrenments(surfaceFormats,
				[](const auto& a, const auto& b) {
					return (static_cast<uint32_t>(a.format) == static_cast<uint32_t>(b.format)) && 
						   (static_cast<uint32_t>(a.colorSpace) == static_cast<uint32_t>(b.colorSpace));
				}))
			{
				selectedFormat = format.value();
			}
			else if (_surfaceFormat.isHardAssignment()) {
				return Ref<VulkanSwapChain>(nullptr);
			}
			else {
				for (const auto& surfaceFormat : surfaceFormats) {
					if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM &&
						surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					{
						selectedFormat = surfaceFormat;
						break;
					}
				}

				selectedFormat = surfaceFormats[0];
			}
		}
	}	
	createInfo.imageFormat = selectedFormat.format;
	createInfo.imageColorSpace = selectedFormat.colorSpace;
	
	return _swapchainExtensionRef->_createSwapChain(createInfo);
}
