
#include <iostream>
#include <stddef.h> 

#include "AntilatencyVulkanCommon.h"
#include "VulkanInstanceFactory.h"
#include "VulkanSurface.h"
#include "InstanceExtensions/WSIInstanceExtension.h"
#include "DeviceExtensions/SwapChainDeviceExtension.h"
#include "QueueConstructors.h"

#include "..\Utils\WindowWin32.h"

#include "Utils/TypeList.h"

int main()
{	
	auto factory = VulkanInstanceFactory::create();

	//Print supported instance extension
	auto instaceExtensions = factory->enumerateExtensionProperties();

	std::cout << "Instance extensions: " << std::endl;
	for (const auto ie : instaceExtensions) {
		std::cout << ie.extensionName << std::endl;
	}

	Ref<VulkanInstance> instance = factory->beginInstanceCreation().
		enableStandardDebug().
		enableExtensions<WSIInstanceExtension, SurfaceInstanceExtension>().
		createInstance();

	if (!instance) {
		std::cout << "Could not create vulkan instance" << std::endl;
	}

	Window window(GetModuleHandle(NULL));
	window.show();

	
	if (auto wsiExtension = instance->getExtension<WSIInstanceExtension>()) {

		if (auto surface = wsiExtension->createSurface(window.inst, window.handle)) {

			auto physicalDevices = instance->enumeratePhysicalDevices();
			
			GraphicsQueueConstructor GQC(1);
			GraphicsQueueConstructor AGQC(1);
			PresentQueueConstructor PQC(surface, 1);
			PQC.setSameWith(GQC);

			for (const Ref<VulkanPhysicalDevice>& physicalDevice : physicalDevices) {
				std::vector<QueueConstructor*> constructors = { &GQC, &AGQC, &PQC };
				if (physicalDevice->distributeQueues( constructors )) {
					if (Ref<VulkanDevice> device = physicalDevice->beginDeviceCreation().
																   declareQueues(constructors).
																   enableExtensions<SwapChainDeviceExtension>().
															       createDevice())
					{
						if (auto swapchainExtension = device->getExtension<SwapChainDeviceExtension>()) {
							if (auto swapchain = swapchainExtension->beginSwapchainCreation(surface).
                                                                     requiredOneOfSurfaceFormats( { VK_FORMAT_R8G8B8A8_SRGB , VK_FORMAT_B8G8R8A8_SRGB } ).
                                                                     requiredSurfaceColorSpace( VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ).
																	 requiredMinImageCount(3).
																	 requiredOneOfPresentModes( { VK_PRESENT_MODE_FIFO_KHR, VK_PRESENT_MODE_MAILBOX_KHR } ).
																	 createSwapChain()) 
							{
								std::cout << std::endl;
							}
						}
						break;
					}
				}
			}


			//auto allPhysicalDevices = instance->enumeratePhysicalDevices();
			//for (size_t i = 0; i < allPhysicalDevices.size(); i++) {
			//	auto queues = surface->getPhysicalDevicePresentQueues(allPhysicalDevices[i]);
			//	if (queues.size() > 0) {
			//		
			//	}
			//}


			//auto selectedPhysicalDevices = make_selector(allPhysicalDevices)
			//	.filterProperties(
			//		[](const auto& properties) {
			//			return std::string(properties.deviceName).find("GeForce") != std::string::npos;
			//		})
			//	.select();



			//auto physicalDevices = make_selector(instance->physicalDevices()).filterProperties([](auto deviceProperties) 
			//{
			//	//...
			//}).filterName([](auto name) 
			//{

			//}).filterExtensions< /*...*/ >().
			//filterQueueus(gqc, tqc, /*...*/).select();

			//if (not physicalDevices.is_empty()) {
			//	//... Use first technique
			//	auto physicalDevice = physicalDevices[0];
			//}

			//physicalDevices = make_selector(instance->selectPhysicalDevices()).filterProperties([](auto deviceProperties)
			//{
			//	//...
			//}).filterName([](auto name)
			//{

			//}).filterExtensions< /*...*/ >().
			//	filterQueueus(gqc, tqc, /*...*/).select();
			//
			//if (not physicalDevices.is_empty()) {
			//	//... Use first technique
			//	auto physicalDevice = physicalDevices[0];
			//}

			//auto physicalDevices = instance->enumeratePhysicalDevices();
			//std::cout << "physicalDevices (" << physicalDevices.size() << "):" << std::endl;

			//for (size_t i = 0; i < physicalDevices.size(); i++) {
			//	std::cout << physicalDevices[i].getProperties().deviceName << std::endl;
			//	//auto displayProperties = physicalDevices[i].getDisplayPropertiesKHR();


			//	auto extensionProperties = physicalDevices[i].getExtensionProperties();
			//	std::cout << "Extensions (" << extensionProperties.size() << "):" << std::endl;
			//	for (size_t e = 0; e < extensionProperties.size(); e++) {
			//		std::cout << "name: " << extensionProperties[e].extensionName << " specVersion:" << extensionProperties[e].specVersion << std::endl;
			//	}
			//}
		}
	}

	//std::vector<const char*> extensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
	//
	//VkInstanceCreateInfo ci = {};
	//ci.enabledExtensionCount = 2;
	//ci.ppEnabledExtensionNames = extensions.data();

	//using KnownExtensions = TypeList<WSIInstanceExtension, SurfaceInstanceExtension>;

	//auto instance = factory->createInstance<KnownExtensions>(ci);
	

	


	//	auto queueFamilyProperties = physicalDevices[i].getQueueFamilyProperties();
	//	

	//	//std::cout << phisicalDevices[i]. << std::endl;
	//}
	

	int frameIndex = 0;
	while (Window::processMessages()) {
		frameIndex++;
	}
	
	
	
	return 0;
}

