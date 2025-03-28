#pragma once
#include "PhysicsShape.h"
#include <PhysicsExport.h>

class b2Shape;

namespace Nova
{
    class PHYSICS_API PhysicsShape2D : public PhysicsShape<b2Shape>
    {
    public:
        PhysicsShape2D() : PhysicsShape() {}
        using HandleType = PhysicsShape::HandleType;
    };
}
