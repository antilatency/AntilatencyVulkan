#pragma once
template<typename Prototype_>
class VulkanFunction {
public:
	using Prototype = Prototype_;
	Prototype function = nullptr;
    virtual const char* getName() = 0;
	void* getFunctionPointer() { return (void*)function; }
	/*bool isAvailable() {
		return pointer != nullptr;
	}*/

	operator bool() const{
		return function != nullptr;
	}


	/*template<typename Host>
	void load(Host* host) {
		if constexpr (NeedInstance) {
			if (host->instance == nullptr) return;
		}
		function = (Prototype)host->get<vkGetInstanceProcAddr>()(NeedInstance ? host->instance : nullptr, getName());
	}
	*/

};
