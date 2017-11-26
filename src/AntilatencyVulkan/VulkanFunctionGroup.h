#pragma once
#include <tuple>
template<typename... Functions>
class VulkanFunctionGroup {
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
	template<typename Context, int ID>
	void _loadAllFunctions(Context* context, void* instance) {
		if constexpr (ID < std::tuple_size<FunctionsTuple>()) {
			auto& f = std::get<ID>(functions);
			f.load(context, instance);
			_loadAllFunctions<Context,ID + 1>(context, instance);
		}
	}
public:
	template<typename Context>
	void loadAllFunctions(Context* context, void* instance) {
		_loadAllFunctions<Context,0>(context, instance);
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