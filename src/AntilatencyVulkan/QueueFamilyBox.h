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
	
	bool addQueue(Queue* q) {
		//check that family supports queue type
		if (_supportedTypes.count(q->type) == 0) {
			return false;
		}

		//We check can we place queue on the top of the corresponding queue list?
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

	//TODO: rewrite to more sophisticated algorithm
	//Better to matrix.
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

private:
	uint32_t _familyIndex = 0;
	uint32_t _queuesCount = 0;
	std::set<VulkanQueueType> _supportedTypes;
	std::unordered_map<VulkanQueueType, std::vector<Queue*> > _queuesPerType;
};
