#pragma once

#include "AntilatencyVulkanCommon.h"
#include "QueueConstructor.h"

//forward
class VulkanPhysicalDevice;


class QueueGarden {
public:
    QueueGarden(std::vector<QueueConstructor*>& constructors, 
				const VulkanPhysicalDevice& pd, 
				const std::vector<VkQueueFamilyProperties>& properties) :
		_constructors(constructors)
    {
		
        std::vector<FamilyInfo> familyInfos(properties.size());

        for (auto i = 0u; i < properties.size(); i++) {
			familyInfos[i].familyIndex = i;
			familyInfos[i].queueCount = properties[i].queueCount;
		}

		for (const auto& c : constructors) {
			auto indices = c->suitableFamilyIndices(pd);
            for (auto i = 0u; i < indices.size(); i++) {
				familyInfos[i].supportedTypes.insert(c->queueType());
			}
		}
		
		for (const auto& fi : familyInfos) {
			_boxes.push_back(QueueFamilyBox(fi));
		}

		fillQueueList();
	}

	~QueueGarden() {
		for (auto* q : _queues) {
			delete q;
		}
	}

	void clear() {
		for (auto& box : _boxes) {
			box.clear();
		}
	}

	bool addQueue(Queue* q) {
		for (auto& box : _boxes) {
			if (box.addQueue(q)) {
				return true;
			}
		}
		return false;
	}

	auto getQueueList() {
		return _queues;
	}

	void fillQueueList() {
		//Fill array of all nodes
		for (const auto& qc : _constructors) {
			for (int i = 0; i < qc->_minRequested; i++) {
				auto q = new Queue();
				q->owner = qc;
				q->type = qc->_type;

				_queues.push_back(q);
			}
		}

		//Fill node->sameWith (node) property
		for (auto i = 0u; i < _queues.size(); i++) {
			auto queue = _queues[i];
			auto queueConstructorOwner = queue->owner;

			//fill same nodes array of node
			for (auto sameConstructor : queueConstructorOwner->_same) {
				for (auto j = 0u; j < _queues.size(); j++) {
					if (i != j && _queues[j]->owner == sameConstructor) {
						queue->sameWith.insert(_queues[j]);
					}
				}
			}
		}
	}

private:
	std::vector<QueueFamilyBox> _boxes;
	std::vector<QueueConstructor*> _constructors;
	std::vector<Queue*> _queues;
};
