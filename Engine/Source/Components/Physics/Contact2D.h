#pragma once
#include "Math/Vector2.h"

namespace Nova
{
    class RigidBody2D;
    
    struct Contact2D
    {
        Vector2 ImpactPoint;
        Vector2 Normal;
        RigidBody2D* Other;
    };
}
