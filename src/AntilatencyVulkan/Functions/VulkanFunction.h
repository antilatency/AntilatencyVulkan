#pragma once
template<typename Prototype_>
class VulkanFunction {
public:
	using Prototype = Prototype_;
	Prototype function = nullptr;
    virtual const char* getName() = 0;
	void* getFunctionPointer() { return (void*)function; }

	operator bool() const{
		return function != nullptr;
	}

};

#define VulkanInstanceFunction(Name) \
class Name : public VulkanFunction<PFN_##Name> {\
public:\
	const char* getName() override { return #Name; }\
	\
	void load(PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkInstance instance = nullptr) {\
		function = (Prototype)getInstanceProcAddr(instance, getName());\
	}

#define VulkanDeviceFunction(Name) \
class Name : public VulkanFunction<PFN_##Name> {\
public:\
	const char* getName() override { return #Name; }\
	\
	void load(PFN_vkGetDeviceProcAddr getDeviceProcAddr, VkDevice device = nullptr) {\
		function = (Prototype)getInstanceProcAddr(instance, getName());\
	}