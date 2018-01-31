#include "VulkanInstanceFactory.h"
#include "VulkanSurface.h"
#include "InstanceExtensions/WSIInstanceExtension.h"
#include "PhysicalDeviceSelector.h"
#include "QueueConstructors.h"

//#include "VulkanSurfaceFactory.h"
#include <iostream>
#include <stddef.h> 
#include "..\Utils\WindowWin32.h"

#include "Utils/TypeList.h"

//#include "QueueConstructor.h"

//VulkanInstanceFactory instanceFactory;
//VulkanInstance<VulkanInstanceFactory> instance(instanceFactory);

int main()
{	
	auto factory = VulkanInstanceFactory::create();

	//Print supported instance extension
	auto instaceExtensions = factory->enumerateExtensionProperties();

	std::cout << "Instance extensions: " << std::endl;
	for (const auto ie : instaceExtensions) {
		std::cout << ie.extensionName << std::endl;
	}

	auto instance = factory->beginInstanceCreation().
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
			//... define queueus needness

			auto physicalDevices = instance->enumeratePhysicalDevices();

			GraphicsQueueConstructor GQC(5, 12);
			GraphicsQueueConstructor AGQC(8, 12);
			PresentQueueConstructor PQC(surface, 7, 12);
			PQC.setSameWith(GQC);

			for (const auto& physicalDevice : physicalDevices) {
				std::vector<QueueConstructor*> constructors = { &GQC, &AGQC, &PQC};
				if (physicalDevice.distributeQueues( constructors )) {
					;
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
			//	.filterProperties([](const auto& properties){
			//		return std::string(properties.deviceName).find("GeForce") != std::string::npos;
			//	}).select();



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

