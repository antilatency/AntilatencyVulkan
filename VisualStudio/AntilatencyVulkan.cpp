#include "AntilatencyVulkan.h"
#include <iostream>

VulkanInstanceFactory instanceFactory;

VulkanInstance<VulkanInstanceFactory> instance;

int main()
{
    
	//instanceFactory.load();
	
	std::cout << "instanceFactory loaded " << instanceFactory.isAllFunctionsLoaded() << std::endl;


	instance = instanceFactory.createInstanceDefault();


	std::cout << "instance loaded " << instance.isAllFunctionsLoaded() << std::endl;
	
	return 0;
}

