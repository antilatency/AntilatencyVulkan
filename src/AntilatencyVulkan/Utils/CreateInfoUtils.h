#pragma once

#include <optional>
#include <vector>
#include <algorithm>
#include <functional>


template<class T>
struct CreateInfoOption {
    using Value = T;
    using Min = T;
    using Max = T;

    std::vector<T> _values;
    bool _isSoftSetup = false;
    bool _isUnset = true;

	CreateInfoOption() 
	{}

	CreateInfoOption(T value, bool isSoftSetup) {
		_values.clear();
		_values.push_back(value);
		_isSoftSetup = isSoftSetup;
		_isUnset = false;
	}

	template<class Container>
    CreateInfoOption(const Container& values, bool isSoftSetup) {
		std::vector<T> newValues(values.begin(), values.end());
		_values = std::move(newValues);
		_isSoftSetup = isSoftSetup;
		_isUnset = false;
	}

	bool unset() {
		_values.clear();
		_isUnset = true;
	}

	bool isValue() const {
		return _values.size() == 1;
	}

	bool isSoft() const {
		return _isSoftSetup;
	}

	bool isHardAssignment() const {
                return !_isSoftSetup;
	}

	std::optional<T> getFirstSatisfiedRequirements(const T& min, const T& max) {
		for (const auto& v : _values) {
			if (min <= v && v <= max ) {
				return { v };
			}
		}
        return {};
	}

    std::optional<T> getFirstSatisfiedRequirements(const T& min, const T& max, std::function<bool(const Value&, const Min&, const Max&)> comparator) {
		for (const auto& v : _values) {
			if (comparator(v, min, max)) {
				return { v };
			}
		}
		return {};
	}

	std::optional<T> getFirstSatisfiedRequirements(const std::vector<T>& allowedVariants) {
		for (const auto& v : _values) {
			if (std::find(allowedVariants.begin(), allowedVariants.end(), v) != allowedVariants.end()) {
                                return { v };
			}
		}
		return {};
	}

	std::optional<T> getFirstSatisfiedRequirements(const std::vector<T>& allowedVariants, std::function<bool(const T, const T)> equalPredicate) const {
		for (const auto& v : _values) {
			auto it = std::find_if(allowedVariants.begin(), allowedVariants.end(),
			[=](const auto av) {
				return equalPredicate(av, v);
			});

			if (it != allowedVariants.end()) {
				return { v };
			}
		}
		return {};
	}

	template<class U>
	T getClosest(const T& min, const T& max, std::function<U> distanceFunc) {
		std::vector<int32_t> distancesPerValue(_values.size());
		
		for (int i = 0; i < _values.size(); i++) {
			auto value = _values[i];
			distancesPerValue[i] = distanceFunc(value, min, max);
		}
		
		auto closestDistance = std::min_element(distancesPerValue.begin(), distancesPerValue.end());
		auto indexOfClosestValue = std::distance(distancesPerValue.begin(), closestDistance);

		return _values[indexOfClosestValue];
	}


	template<class U>
	T getClosest(const std::vector<T>& allowedVariants, std::function<U> distanceFunc) {
		using Distance = int32_t;
		using IndexOfAlwailableValue = uint32_t;
		using ValueToAllowedDistanceEntry = std::pair<Distance, IndexOfAlwailableValue>;

		std::vector<ValueToAllowedDistanceEntry> distancesPerValue;

		for (const auto& value : _values) {

			std::vector<Distance> distancesToAllowed;

			for (const auto& allowed : allowedVariants) {
				distancesToAllowed.push_back( distanceFunc(value, allowed) );
			}

			auto closestDistanceToAllowed = std::min_element(distancesToAllowed.begin(), distancesToAllowed.end());
			auto indexOfClosestValue = std::distance(distancesToAllowed.begin(), closestDistanceToAllowed);

			distancesPerValue.push_back(ValueToAllowedDistanceEntry((*closestDistanceToAllowed), indexOfClosestValue));
		}

		auto closestDistance = std::min_element(distancesPerValue.begin(), distancesPerValue.end(),
			[](const auto& left, const auto& right) {
				return left.first < right.first;
			});
		auto indexOfClosestValue = (*closestDistance).second;

		return _values[indexOfClosestValue];
	}

	std::optional<T> selectValue(const std::vector<T> allowedVariants, std::function<uint32_t(const T&, const T&)> distanceFunc) {
		if (allowedVariants.size() == 0 || _values.size() == 0 || isUnset()) {
			return {};
		}

		auto val = getFirstSatisfiedRequirements(allowedVariants);
		if (val) {
			return val.value();
		}
		else {
			if (isHardAssignment()) {
				return {};
			}
			else {
				return { getClosest(allowedVariants, distanceFunc) };
			}
		}
	}

	//template<class U>
	std::optional<T> selectValue(const T& min, const T& max, std::function<uint32_t(const T&, const T&, const T&)> distanceFunc) {
		if (allowedVariants.size() == 0 || _values.size() == 0 || isUnset()) {
			return {};
		}

		auto val = getFirstSatisfiedRequirements(min, max);
		if (val) {
			return val.value();
		}
		else {
			if (isHardAssignment()) {
				return {};
			}
			else {
				return { getClosest(min, max, distanceFunc) };
			}
		}
	}
	
	auto isUnset() const {
		return _isUnset;
	}
};
