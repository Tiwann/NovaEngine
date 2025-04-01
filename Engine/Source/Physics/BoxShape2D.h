#pragma once
#include "PhysicsShape2D.h"
#include "Math/Vector2.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API BoxShape2D : public PhysicsShape2D
    {
    public:
        explicit BoxShape2D(const Vector2& HalfExtents, const Vector2& Center, f32 Rotation);
    private:
        Vector2 m_HalfExtents;
        Vector2 m_Center;
        f32 m_Rotation = 0.0f;
    };
}
