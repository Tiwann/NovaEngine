#pragma once
#include "Math/Vector3.h"

namespace Nova
{
    class PhysicsBody2D;

    struct PhysicsContact
    {
        Vector3 point;
        Vector3 normal;
        PhysicsBody* otherBody = nullptr;
    };
}
