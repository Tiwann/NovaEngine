#pragma once
#include "Math/Vector3.h"
#include "Runtime/Types.h"

namespace Nova
{
    class Physics3D
    {
    public:
        static Vector3 Gravity;
        static f32 TimeStep;
        static constexpr u32 MaxBodies = 0xFFFFui32;
        static constexpr u32 MaxBodyPairs = 0xFFFFui32;
        static constexpr u32 MaxContactConstraints = 10240;
    };
}
