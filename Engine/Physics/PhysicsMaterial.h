#pragma once
#include <PhysicsExport.h>

namespace Nova
{
    struct PHYSICS_API PhysicsMaterial
    {
        f32 Density = 1.0f;
        f32 Friction = 0.0f;
        f32 Bounciness = 0.0f;
    };
}
