#pragma once

#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include <cassert>


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

class QueueConstructor;

struct Queue {
	VulkanQueueType type;
	std::set<Queue*> sameWith;
	QueueConstructor* owner = nullptr;
	int32_t familyIndex = -1;
	int32_t queueIndex = -1;
};


struct FamilyInfo {
	uint32_t familyIndex;
	uint32_t queueCount;
	std::set<VulkanQueueType> supportedTypes;
};


class QueueFamilyBox {
public:
	/*QueueFamilyBox(const std::vector<VulkanQueueType>& supportedTypes, uint32_t familyIndex, uint32_t queuesCount)  :
                _familyIndex(familyIndex),
                _queuesCount(queuesCount),
                _supportedTypes(supportedTypes)
	{
		for (const auto type : _supportedTypes) {
			_queuesPerType[type] = std::vector<Queue*>(_queuesCount);
		}
	}*/

        QueueFamilyBox(const FamilyInfo& info) :
		_familyIndex(info.familyIndex),
                _queuesCount(info.queueCount),
                _supportedTypes(info.supportedTypes)
	{
		for (const auto type : _supportedTypes) {
			_queuesPerType[type] = std::vector<Queue*>(_queuesCount);
		}
	}

	bool supportType(VulkanQueueType type) {
		return std::find(std::begin(_supportedTypes), std::end(_supportedTypes), type) != std::end(_supportedTypes);
	}

	void clear() {
        for(const auto type : _supportedTypes) {
            auto& v = _queuesPerType[type];
            for(auto& q : v) {
                q = nullptr;
            }
        }
	}
	
	//bool addQueue(Queue* q) {
	//	//check that family supports queue type
	//	//assert(std::find(std::begin(_supportedTypes), std::end(_supportedTypes), q->type) != std::end(_supportedTypes));
	//	if (std::find(std::begin(_supportedTypes), std::end(_supportedTypes), q->type) == std::end(_supportedTypes)) {
	//		return false;
	//	}

	//	//We grab last free index in array of coresponding queue type in that family.
	//	//In other words we grab last unassigned index of queue index in family.

	//	auto& queueVector = _queuesPerType[q->type];
 //       auto queuePlace = lastFreePlace(q->type);

	//	//If grabbed index is out of scope in that family, we notify user that we could not place this queue in that family
	//	if (queuePlace == _queuesCount) {
	//		return false;
	//	}

	//	//If queue is first queue in that family, place it at bottom of the box
	//	if (queuePlace == 0) {
	//		queueVector[0] = q;
	//		return true;
	//	}

	//	queueVector[queuePlace] = q;

	//	//if there is an empty place to move down for a queue

	//	//We start moving down a queue.
	//	//If 
	//	queuePlace--;

 //       while (queueVector[queuePlace] == nullptr && allQueuesOnIndexCanBeSame(*q, queuePlace)) {
	//		//move down queue
	//		queueVector[queuePlace + 1] = nullptr;
	//		queueVector[queuePlace] = q;

	//		if(queuePlace != 0) {
	//			queuePlace--;
 //           }
 //           else {
	//			break;
	//		}
	//	}	
	//	return true;
	//}

	bool addQueue(Queue* q) {
		//check that family supports queue type
		//assert(std::find(std::begin(_supportedTypes), std::end(_supportedTypes), q->type) != std::end(_supportedTypes));
		/*if (std::find(std::begin(_supportedTypes), std::end(_supportedTypes), q->type) == std::end(_supportedTypes)) {
			return false;
		}*/
		if (_supportedTypes.count(q->type) == 0) {
			return false;
		}

		//We check can we place queue on the top of the coresponding queue list?
		//If not, we notify user that we can not place the queue, else we do so.
		auto& queueVector = _queuesPerType[q->type];
		if (queueVector.back() != nullptr) {
			//queue vector is full
			return false;
		}
		else {
			q->familyIndex = _familyIndex;
			queueVector.back() = q;
		}

		//Check special case
		if (queueVector.size() < 2) {
			// only 1 possible queue in family. We try to assign earlier.
			q->queueIndex = 0;
			return true; 
		}

		int32_t queuePlace = queueVector.size() - 2;
		for (; queuePlace >= 0; --queuePlace) {
			//if there is no queue in the bottom of our queue
			//So we can move
			if (queueVector[queuePlace] == nullptr && allQueuesOnIndexCanBeSame(*q, queuePlace)) {
					//We move our queue down
					queueVector[queuePlace] = q;
					queueVector[queuePlace + 1] = nullptr;
			}
			else {
				break;
			}
		}
		q->queueIndex = queuePlace + 1;
		return true;
	}

    bool allQueuesOnIndexCanBeSame(const Queue& q, const uint32_t index) {
		//Check if ALL queues on that place might be the same with our new queue
		for (const auto type : _supportedTypes) {
			const auto& vec = _queuesPerType[type];
			if (vec[index] != nullptr) {
				auto& queue = vec[index];

				if (std::find(queue->sameWith.begin(), queue->sameWith.end(), &q) == queue->sameWith.end()) {
					return false;
				}
			}
		}
		return true;
	}

	std::size_t lastFreePlace(const VulkanQueueType type) {
		const auto& queueVector = _queuesPerType[type];
		for (int j = _queuesCount - 1; j >= 0; j--) {
			if (queueVector[j] != nullptr) {
				return j + 1;
			}
		}
		return 0;
	}

private:
	uint32_t _familyIndex = 0;
	uint32_t _queuesCount = 0;
	std::set<VulkanQueueType> _supportedTypes;
	std::unordered_map<VulkanQueueType, std::vector<Queue*> > _queuesPerType;
};
