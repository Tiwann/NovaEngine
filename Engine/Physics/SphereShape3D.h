#pragma once
#include "PhysicsShape3D.h"

namespace Nova
{
    class SphereShape3D : public PhysicsShape3D
    {
    public:
        explicit SphereShape3D(f32 Radius);
    private:
        f32 m_Radius;
    };
}
