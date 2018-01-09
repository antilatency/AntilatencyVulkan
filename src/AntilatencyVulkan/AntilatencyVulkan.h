#pragma once
#define VK_NO_PROTOTYPES
#include "vulkan.h"

template < class T>
class Ref;
template < class T>
auto makeRef(T* object);

template<typename T>
class Ref {
private:
	T* pointer = nullptr;

//private:
//	friend auto makeRef<T>(T*);

	
public:
	/*template <typename ... Args>
	Ref(Args...args) {
		pointer = new T(args...);
		pointer->AddRef();
	}*/
	
	Ref(T* pointer) :pointer(pointer) {
		pointer->AddRef();
	}
	Ref(const Ref &other) {
		pointer = other.pointer;
		pointer->AddRef();
	}

	T* operator -> () { return pointer; }

	~Ref() {
		pointer->Release();
	}
};


/*

template <typename T>
auto makeRef(T* object) {
	return Ref<T>(object);
}*/

class RefCounter {
protected:
	friend class Ref<RefCounter>; //For AbstractRef
	int refCount = 0;
	RefCounter() {};
	virtual ~RefCounter() {}

	void AddRef() {
		refCount++;
	}

	int Release() {
		refCount--;
		if (refCount == 0) {
			delete this;
			return 0;
		}
		return refCount;
	}

};
using AbstractRef = Ref<RefCounter>;

#include "VulkanInstanceFactory.h"
#include "VulkanInstance.h"
