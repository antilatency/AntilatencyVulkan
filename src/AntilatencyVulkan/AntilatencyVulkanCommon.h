#pragma once
#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan.h"

#include <tuple>
#include <vector>

template < class T>
class Ref;
template < class T>
auto makeRef(T* object);

template<class T>
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

	T* operator -> () const { return pointer; }
	T* ptr() const { return pointer; }

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
    RefCounter() {}
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

template<typename ... Args, typename ... EnumeratorArgs>
auto vulkanEnumerate(VkResult(VKAPI_PTR *enumerator)(EnumeratorArgs...enumeratorArgs), Args...args) {
    using Tuple = std::tuple<EnumeratorArgs...>;

    using ResultType = typename std::remove_pointer<
        typename std::tuple_element<std::tuple_size<Tuple>::value - 1, Tuple>::type
	>::type;
	std::vector<ResultType> result;

	uint32_t count = 0;
	if (VK_SUCCESS == enumerator(args..., &count, nullptr)) {
		VkResult r;
		do {
			result.resize(count);
			r = enumerator(args..., &count, result.data());
			if ((r != VK_SUCCESS) && (r != VK_INCOMPLETE)) return std::vector<ResultType>();
		} while (r == VK_INCOMPLETE);

	}
	return result;
}

template<typename ... Args, typename ... EnumeratorArgs>
auto vulkanEnumerate(void(VKAPI_PTR *enumerator)(EnumeratorArgs...enumeratorArgs), Args...args) {
	using Tuple = std::tuple<EnumeratorArgs...>;
    using ResultType = typename std::remove_pointer<
        typename std::tuple_element<std::tuple_size<Tuple>::value - 1, Tuple>::type
	>::type;

	uint32_t count = 0;
	enumerator(args..., &count, nullptr);
	std::vector<ResultType> result(count);
	enumerator(args..., &count, result.data());

	return result;
}

template<class U>
Ref<U> ref_dynamic_cast(const Ref<RefCounter>& ref) {
	return Ref<U>(dynamic_cast<U*>(ref.operator->()));
}


template<class U, class T>
Ref<U> ref_dynamic_cast(const Ref<T>& ref) {
	//TODO: assert(rawPointer, "invalid cast")?
	return Ref<U>(dynamic_cast<U*>(ref.operator->()));
}


template<class U, class T>
Ref<U> ref_is(const Ref<T>& ref) {
	return ( dynamic_cast<U*>(ref.operator->()) != nullptr );
}


template<class T, class U>
Ref<T> ref_static_cast(const Ref<U>& ref) {
	return Ref<T>( static_cast<T*>(ref.operator->()) );
}


template<typename T, typename... Args>
Ref<T> make_ref(Args&&... args) {
	return Ref<T>(new T(std::forward<Args>(args)...));
}


//#include "Functions/VulkanFunctionGroup.h"
//#include "Functions/VulkanFunction.h"
//
//#include "VulkanInstanceFactory.h"
//#include "VulkanInstance.h"
