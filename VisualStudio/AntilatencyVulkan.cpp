#include "VulkanInstanceFactory.h"
#include <iostream>
#include <stddef.h> 
#include "..\Utils\WindowWin32.h"


//VulkanInstanceFactory instanceFactory;
//VulkanInstance<VulkanInstanceFactory> instance(instanceFactory);

template <typename QueueType>
class QueueArray {

};

class GraphicsQueueConstructor {

};

class PresentQueueConstructor {


};

class TransferQueueConstructor {


};

int main()
{
	
	GraphicsQueueConstructor gr;
	PresentQueueConstructor pr;

	
	
	auto factory = VulkanInstanceFactory::create();

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

