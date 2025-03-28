#pragma once
#include "PhysicsContactInfo.h"
#include "Math/Vector3.h"
#include <PhysicsExport.h>

namespace Nova
{
    class PhysicsBody3D;
    struct PHYSICS_API PhysicsContactInfo3D : PhysicsContactInfo<PhysicsBody3D, Vector3>{};
}