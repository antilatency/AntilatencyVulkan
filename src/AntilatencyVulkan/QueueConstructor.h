#pragma once

#include <set>
#include <vector>
#include <memory>
#include <algorithm>
#include <iterator>

#include "QueueFamilyBox.h"

//Forward declaration
class VulkanPhysicalDevice;

class QueueConstructor {
public:
	using FamilyIndex = uint32_t;
	using QueueIndexInFamily = uint32_t;
	using QueueIndexPair = std::pair<FamilyIndex, QueueIndexInFamily>;
	using QueueFamilyIndexContainer = std::set<QueueIndexPair>;

	virtual void fill(const VulkanPhysicalDevice& physicalDevice) = 0;
	virtual std::vector<uint32_t> suitableFamilyIndices(const VulkanPhysicalDevice& device) = 0;


	bool sameWith(const QueueConstructor* other) const {
		/*for (auto q : _queues) {
			for (auto oq : other->_queues) {
				if ( !(q->sameWith.count(oq) && oq->sameWith.count(q)) ) {
					return false;
				}
			}
		}
		return true;*/
		return std::find(_same.begin(), _same.end(), other) != _same.end();
	}

	void setSameWith(QueueConstructor& other) {
		/*for (auto q : _queues) {
			for (auto oq : other->_queues) {
				q->sameWith.insert(oq);
				oq->sameWith.insert(q);
			}
		}*/
        if (std::find(_same.begin(), _same.end(), &other) == _same.end()) {
			_same.push_back(&other);
			other._same.push_back(this);
		}
	}

	auto queueType() {
		return _type;
	}

	QueueConstructor(VulkanQueueType type, int min, int max) : 
		_minRequested(min), 
        _maxRequested(max),
        _type(type)
	{}

    virtual ~QueueConstructor() {
		//remove same reference
		for (auto constructor : _same) {
			constructor->_same.erase(std::remove(constructor->_same.begin(), constructor->_same.end(), this));
		}
	}

	VulkanQueueType _type;
	int _minRequested = 0;
	int _maxRequested = 0;
	std::vector<QueueConstructor*> _same;
	QueueFamilyIndexContainer _queueIndexes;
};

