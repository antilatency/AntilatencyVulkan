#pragma once

#include <set>
#include <memory>
#include <algorithm>
#include <iterator>


//template <typename QueueType>
//class QueueArray {
//
//};
//

enum class VulkanQueueType : uint32_t {
	GRAPHICS = 1,
	PRESENT = 2,
	TRANSFER = 4,
	COMPUTE = 8
	//...
};

VulkanQueueType& operator |=(VulkanQueueType &lhs, VulkanQueueType rhs)
{
	lhs = static_cast<VulkanQueueType> (
		static_cast<std::underlying_type<VulkanQueueType>::type>(lhs) |
		static_cast<std::underlying_type<VulkanQueueType>::type>(rhs)
		);

	return lhs;
}

VulkanQueueType& operator &=(VulkanQueueType &lhs, VulkanQueueType rhs)
{
	lhs = static_cast<VulkanQueueType> (
		static_cast<std::underlying_type<VulkanQueueType>::type>(lhs) &
		static_cast<std::underlying_type<VulkanQueueType>::type>(rhs)
		);

	return lhs;
}

VulkanQueueType operator ~(VulkanQueueType rhs)
{
	return static_cast<VulkanQueueType> (
		~static_cast<std::underlying_type<VulkanQueueType>::type>(rhs)
		);
}

VulkanQueueType operator &(VulkanQueueType lhs, VulkanQueueType rhs)
{
	return static_cast<VulkanQueueType> (
		static_cast<std::underlying_type<VulkanQueueType>::type>(lhs) &
		static_cast<std::underlying_type<VulkanQueueType>::type>(rhs)
		);
}

//Forward declaration
class VulkanPhysicalDevice;

class QueueConstructor {
public:
	using FamilyIndex = uint32_t;
	using QueueIndexInFamily = uint32_t;
	using QueuFamilyIndexContainer = std::set<std::pair<FamilyIndex, QueueIndexInFamily>>;

    virtual ~QueueConstructor() = default;

	virtual void fill(const VulkanPhysicalDevice& physicalDevice) = 0;

	auto findIntersections(const QueueConstructor& other) {
		QueuFamilyIndexContainer output;
		/*std::set_intersection(_queueFamilyIndices.begin(), _queueFamilyIndices.end(),
			constructor._queueFamilyIndices, constructor._queueFamilyIndices,
			std::inserter(output, output.begin()));*/

		for (const auto& myIndices : _queueFamilyIndices) {
			for (const auto& otherIndices : other._queueFamilyIndices) {
				if ((myIndices.first == otherIndices.first) && (myIndices.second == otherIndices.second)) {
					output.insert(otherIndices);
				}
			}
		}
		return output;
	}

	void removeQueueIndexes(const QueuFamilyIndexContainer& indexes) {
		/*QueuFamilyIndexContainer output;
		std::set_difference(_queueFamilyIndices.begin(), _queueFamilyIndices.end(),
			indexes, indexes,
			std::inserter(output, output.begin()));

        _queueFamilyIndices = std::move(output);*/

		for (const auto& p : indexes) {
			if (_queueFamilyIndices.count(p) > 0) {
				_queueFamilyIndices.erase(p);
			}
		}
	}

	void setCanBeSame(QueueConstructor& other, bool flag) {
		if (flag == true) {
			_sameWith |= other._type;
			other._sameWith |= _type;
		}
		else {
			_sameWith &= (~other._type);
			other._sameWith &= (~_type);
		}
	}

	void expandWithQueueFamilies(const QueuFamilyIndexContainer& indexes, int count = -1) {
		/*QueuFamilyIndexContainer output;
		std::set_union(_queueFamilyIndices.begin(), _queueFamilyIndices.end(),
			indexes, indexes,
			std::inserter(output, output.begin()));
        _queueFamilyIndices = std::move(output);*/

		if (count == -1) {
			count = indexes.size();
		}

		for (const auto& indicesPair : indexes) {
			if (count > 0) {
                auto it = _queueFamilyIndices.insert(indicesPair);
				if (it.second == true) {//insertion took place
					count -= 1;
				}
			}
		}
	}

	bool canBeSame(const QueueConstructor& other) const {
		return static_cast<uint32_t>(_sameWith & other._type) > 0;
	}

	QueueConstructor(VulkanQueueType type, int min, int max) : 
		_minRequested(min), 
        _maxRequested(max),
        _sameWith(type),
        _type(type)
	{}

	int _minRequested = 0;
	int _maxRequested = 0;
	VulkanQueueType _sameWith;
	VulkanQueueType _type;

	QueuFamilyIndexContainer _queueFamilyIndices;
};

