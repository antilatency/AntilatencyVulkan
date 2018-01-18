#include "VulkanInstanceFactory.h"
#include <iostream>
#include <stddef.h> 
#include "..\Utils\WindowWin32.h"

//#include "QueueConstructor.h"

//VulkanInstanceFactory instanceFactory;
//VulkanInstance<VulkanInstanceFactory> instance(instanceFactory);



int main()
{
	
	//GraphicsQueueConstructor gr;
	//PresentQueueConstructor pr;
	
	auto factory = VulkanInstanceFactory::create();

	//Print supported instance extension
	auto instaceExtensions = factory->enumerateExtensionProperties();

	std::cout << "Instance extensions: " << std::endl;
	for (const auto ie : instaceExtensions) {
		std::cout << ie.extensionName << std::endl;
	}


	auto instance = factory->createInstance(factory->getDebugCreateInfo());
	
	auto physicalDevices = instance->enumeratePhysicalDevices();
	std::cout << "physicalDevices ("<< physicalDevices.size() <<"):" << std::endl;

	for (size_t i = 0; i < physicalDevices.size(); i++) {
		std::cout << physicalDevices[i].getProperties().deviceName << std::endl;
		//auto displayProperties = physicalDevices[i].getDisplayPropertiesKHR();


		auto extensionProperties = physicalDevices[i].getExtensionProperties();
		std::cout << "Extensions (" << extensionProperties.size() << "):" << std::endl;
		for (size_t e = 0; e < extensionProperties.size(); e++) {
			std::cout << "name: " << extensionProperties[e].extensionName << " specVersion:" << extensionProperties[e].specVersion << std::endl;
		}


		auto queueFamilyProperties = physicalDevices[i].getQueueFamilyProperties();
		

		//std::cout << phisicalDevices[i]. << std::endl;
	}



	Window window(GetModuleHandle(NULL));
	window.show();

	int frameIndex = 0;
	while (Window::processMessages()) {
		std::cout << frameIndex;
		frameIndex++;
	}
	



	
	
	return 0;
}

