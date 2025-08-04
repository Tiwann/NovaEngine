#pragma once
#include "Runtime/Flags.h"

namespace Nova
{
    enum class PhysicsConstraintsFlagBits
    {
        None = 0,
        PositionX = BIT(0),
        PositionY = BIT(1),
        PositionZ = BIT(2),
        RotationX = BIT(3),
        RotationY = BIT(4),
        RotationZ = BIT(5),
        Position = PositionX | PositionY | PositionZ,
        Rotation = RotationX | RotationY | RotationZ,
    };

    typedef Flags<PhysicsConstraintsFlagBits> PhysicsConstraintsFlags;
}
