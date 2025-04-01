#pragma once
#include "Math/Vector3.h"
#include "PhysicsBodyType.h"
#include <CoreExport.h>

namespace Nova
{
    struct PhysicsBodyDefinition
    {
        Vector3 Position;
        Vector3 Rotation;
        PhysicsBodyType Type;
        bool IsTrigger;
    };
}
