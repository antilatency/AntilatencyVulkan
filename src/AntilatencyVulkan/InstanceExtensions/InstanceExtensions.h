#pragma once

#include "InstanceExtension.h"
#include "SurfaceInstanceExtension.h"
#include "WSIInstanceExtension.h"

#include "Utils/TypeList.h"

using DefaultInstanceExtensions = TypeList<WSIInstanceExtension, SurfaceInstanceExtension>;