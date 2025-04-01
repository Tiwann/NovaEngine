#pragma once
#include "Runtime/Flags.h"

namespace Nova
{
    enum class PhysicsConstraintsBits
    {
        None = 0,
        NOVA_FLAG_BIT(PositionX, 0),
        NOVA_FLAG_BIT(PositionY, 1),
        NOVA_FLAG_BIT(PositionZ, 2),
        NOVA_FLAG_BIT(RotationX, 3),
        NOVA_FLAG_BIT(RotationY, 4),
        NOVA_FLAG_BIT(RotationZ, 5),
        Position = PositionX | PositionY | PositionZ,
        Rotation = RotationX | RotationY | RotationZ,
        All = Position | Rotation,
    };

    NOVA_DECLARE_FLAGS(PhysicsConstraintsBits, PhysicsConstraintsFlags)
}
