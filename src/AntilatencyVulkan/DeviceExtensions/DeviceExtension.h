#pragma once

#include "AntilatencyVulkanCommon.h"
#include <string>

class VulkanDevice;

class DeviceExtension : public RefCounter {
	friend class Ref<DeviceExtension>;

public:
	DeviceExtension(const Ref<VulkanDevice>& vulkanDevice) :
		_deviceRef(vulkanDevice)
	{}

	virtual ~DeviceExtension() = default;

protected:
	Ref<VulkanDevice> _deviceRef;
};
