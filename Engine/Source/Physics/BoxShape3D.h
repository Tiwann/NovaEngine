#pragma once
#include "PhysicsShape3D.h"
#include "Math/Vector3.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API BoxShape3D : public PhysicsShape3D
    {
    public:
        explicit BoxShape3D(Vector3 Center, Vector3 HalfExtents, Vector3 Rotation);
    private:
        Vector3 m_Center;
        Vector3 m_HalfExtents;
        Vector3 m_Rotation;
    };
}
