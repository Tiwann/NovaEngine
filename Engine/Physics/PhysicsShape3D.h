#pragma once
#include "PhysicsShape.h"
#include <PhysicsExport.h>

namespace JPH { class Shape; }

namespace Nova
{
    class PHYSICS_API PhysicsShape3D : public PhysicsShape<JPH::Shape>
    {
    public:
        using HandleType = PhysicsShape::HandleType;
        PhysicsShape3D() : PhysicsShape() { }
    };
}
