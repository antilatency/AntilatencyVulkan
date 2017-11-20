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
protected:
	template<typename Context, int ID = 0>
	void load(Context* context, void* instance) {
		if constexpr (ID < std::tuple_size<FunctionsTuple>()) {
			auto& f = std::get<ID>(functions);
			f.load(context, instance);
			load<Context,ID + 1>(context, instance);
		}
	}
public:
	template<typename Context>
	void loadAll(Context* context, void* instance) {
		load<Context,0>(context, instance);
	}

protected:
	template<int ID = 0>
	bool isAllFunctionsLoaded() {
		if constexpr (ID < std::tuple_size<FunctionsTuple>()) {
			auto& f = std::get<ID>(functions);
			if (!f) return false;
			return isAllFunctionsLoaded<ID + 1>();
		}
		return true;
	}
public:
	bool isAllFunctionsLoaded() {
		return isAllFunctionsLoaded<0>();
	}

};