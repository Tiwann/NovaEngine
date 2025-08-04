#pragma once
#include "Math/Vector3.h"
#include "PhysicsBodyType.h"
#include "Math/Quaternion.h"

namespace Nova
{
    struct PhysicsBodyDefinition
    {
        Vector3 position = Vector3::Zero;
        Quaternion rotation = Quaternion::Identity;
        PhysicsBodyType type = PhysicsBodyType::Static;
        bool isTrigger;
    };
}
