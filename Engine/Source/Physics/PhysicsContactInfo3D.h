#pragma once
#include "PhysicsContactInfo.h"
#include "Math/Vector3.h"
#include <CoreExport.h>

namespace Nova
{
    class PhysicsBody3D;
    struct CORE_API PhysicsContactInfo3D : PhysicsContactInfo<PhysicsBody3D, Vector3>{};
}