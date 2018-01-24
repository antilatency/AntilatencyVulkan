#pragma once

class InstanceExtensionFactory
{
	InstanceExtensionFactory() {

	}

	virtual ~InstanceExtensionFactory() = default;

	template<class Container>
	auto createExtension(const Container& c) {

	}
};