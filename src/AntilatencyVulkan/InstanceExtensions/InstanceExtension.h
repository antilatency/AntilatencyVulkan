#pragma once

#include "AntilatencyVulkanCommon.h"
#include <string>

class VulkanInstance;

class InstanceExtension : public RefCounter {
    friend class Ref<InstanceExtension>;
public:
	using InstanceExensionTypeId = std::string;

public:
	InstanceExtension(const Ref<VulkanInstance>& vulkanInstance) :
		_instanceRef(vulkanInstance)
	{}

	virtual ~InstanceExtension() = default;

	virtual InstanceExensionTypeId extensionTypeId() = 0;

protected:
	Ref<VulkanInstance> _instanceRef;
};
