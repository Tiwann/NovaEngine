#pragma once
#include "PhysicsShape2D.h"
#include "Math/Vector2.h"
#include <PhysicsExport.h>

namespace Nova
{
    class PHYSICS_API CircleShape2D : public PhysicsShape2D
    {
    public:
        explicit CircleShape2D(f32 Radius, const Vector2& Center);
    private:
        f32 m_Radius;
        Vector2 m_Center;
    };
}
