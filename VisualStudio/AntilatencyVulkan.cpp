#include "AntilatencyVulkan.h"
#include <iostream>

VulkanInstanceFactory instanceFactory;
VulkanInstance<VulkanInstanceFactory> instance(instanceFactory);

int main()
{
    
	//instanceFactory.load();
	
	std::cout << "instanceFactory loaded " << instanceFactory.isAllFunctionsLoaded() << std::endl;


	auto layers =  instanceFactory.get<vkEnumerateInstanceLayerProperties>().asVector();

	instance = instanceFactory.createInstanceDebug();

	


	std::cout << "instance loaded " << instance.isAllFunctionsLoaded() << std::endl;
	
	return 0;
}

