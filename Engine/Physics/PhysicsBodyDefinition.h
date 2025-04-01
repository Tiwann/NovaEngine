#pragma once
#include "Math/Vector3.h"
#include "PhysicsBodyType.h"
#include <PhysicsExport.h>

namespace Nova
{
    struct PHYSICS_HIDDEN PhysicsBodyDefinition
    {
        Vector3 Position;
        Vector3 Rotation;
        PhysicsBodyType Type;
        bool IsTrigger;
    };
}
