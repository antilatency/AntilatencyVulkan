#pragma once

#include "QueueConstructor.h"
#include "QueueFamilyBox.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"

class GraphicsQueueConstructor : public QueueConstructor {
public:
	GraphicsQueueConstructor(int min, int max) :
		QueueConstructor(VulkanQueueType::GRAPHICS, min, max)
	{}

	void fill(const VulkanPhysicalDevice& physicalDevice) override {
		//auto properties = physicalDevice.getQueueFamilyProperties();

		//for (auto familyIndex = 0; familyIndex < properties.size(); familyIndex++) {
		//	//FIXME: is that realy need to check queueCount > 0
		//	auto& p = properties[familyIndex];
		//	if (p.queueCount > 0 && p.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
		//		for (auto queueInFamilyIndex = 0; queueInFamilyIndex < p.queueCount; queueInFamilyIndex++) {
		//			auto.insert(std::make_pair(familyIndex, queueInFamilyIndex));
		//		}
		//	}
		//}
	}

	std::vector<uint32_t> suitableFamilyIndices(const VulkanPhysicalDevice& device) override {
		auto properties = device.getQueueFamilyProperties();
		std::vector<uint32_t> out;

		for (auto familyIndex = 0; familyIndex < properties.size(); familyIndex++) {
			auto& p = properties[familyIndex];
			if (p.queueCount > 0 && p.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				out.push_back(familyIndex);
			}
		}

		return out;
	}
};


class PresentQueueConstructor : public QueueConstructor {
public:
	PresentQueueConstructor(const Ref<VulkanSurface>& surface, int min, int max) :
		QueueConstructor(VulkanQueueType::PRESENT, min, max),
		_surface(surface)
	{}

    void fill(const VulkanPhysicalDevice& physicalDevice) override {
        /*auto presentIndices = _surface->getPhysicalDevicePresentQueueFamiliyIndices(physicalDevice);
		auto properties = physicalDevice.getQueueFamilyProperties();

		for (auto familyIndex = 0; familyIndex < properties.size(); familyIndex++) {
			auto& p = properties[familyIndex];
			if (p.queueCount > 0 && std::find(presentIndices.begin(), presentIndices.end(), familyIndex) != presentIndices.end()) {
				for (auto queueInFamilyIndex = 0; queueInFamilyIndex < p.queueCount; queueInFamilyIndex++) {
					_queueFamilyIndices.insert(std::make_pair(familyIndex, queueInFamilyIndex));
				}
			}
		}*/
    }

	std::vector<uint32_t> suitableFamilyIndices(const VulkanPhysicalDevice& device) override {
		return _surface->getPhysicalDevicePresentQueueFamiliyIndices(device);
	}

private:
	Ref<VulkanSurface> _surface;
};
