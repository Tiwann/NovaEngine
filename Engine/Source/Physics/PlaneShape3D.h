#pragma once
#include "PhysicsShape3D.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include <CoreExport.h>


namespace Nova
{
    class CORE_API PlaneShape3D : public PhysicsShape3D
    {
    public:
        explicit PlaneShape3D(Vector3 Center, Quaternion Rotation);
    private:
        Vector3 m_Center;
        Quaternion m_Rotation;
    };
}
