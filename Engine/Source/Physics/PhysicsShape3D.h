#pragma once
#include "PhysicsShape.h"
#include <CoreExport.h>

namespace JPH { class Shape; }

namespace Nova
{
    class CORE_API PhysicsShape3D : public PhysicsShape<JPH::Shape>
    {
    public:
        using HandleType = PhysicsShape::HandleType;
        PhysicsShape3D() : PhysicsShape() { }
    };
}
