#pragma once

#include <tuple>


class VulkanFunctionGroupBase : public RefCounter {
	friend class Ref<VulkanFunctionGroupBase>;
};



template<typename... Functions>
class VulkanFunctionGroup : public VulkanFunctionGroupBase {
	
	friend class Ref< VulkanFunctionGroup >;

	using FunctionsTuple = std::tuple<Functions...>;
	FunctionsTuple functions;
public:
	template<typename F>
	F& get() {
		return std::get<F>(functions);
	}

	template<typename ...T>
	friend class VulkanFunctionGroup;
private:
	template<typename LoaderFunction, int ID, typename ... AdditionalParameters>
	void _load(LoaderFunction loaderFunction, AdditionalParameters... additionalParameters) {
		if constexpr (ID < std::tuple_size<FunctionsTuple>()) {
			auto& f = std::get<ID>(functions);
			f.load(loaderFunction, additionalParameters...);
			//
			_load<LoaderFunction,ID + 1>(loaderFunction, additionalParameters...);
		}
	}
public:
	template<typename LoaderFunction, typename ... AdditionalParameters>
	void load(LoaderFunction loaderFunction, AdditionalParameters... additionalParameters) {
		_load<LoaderFunction,0>(loaderFunction, additionalParameters...);
	}

private:
	template<int ID = 0>
	bool _isAllFunctionsLoaded() {
		if constexpr (ID < std::tuple_size<FunctionsTuple>()) {
			auto& f = std::get<ID>(functions);
			if (!f) return false;
			return _isAllFunctionsLoaded<ID + 1>();
		}
		return true;
	}
public:
	bool isAllFunctionsLoaded() {
		return _isAllFunctionsLoaded<0>();
	}

};