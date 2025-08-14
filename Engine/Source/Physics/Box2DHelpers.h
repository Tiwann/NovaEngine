#pragma once
#include "PhysicsConstraints.h"
#include "box2d/math_functions.h"
#include "box2d/types.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"


namespace Nova
{
    inline b2Vec2 Tob2Vec2(const Vector2& vector)
    {
        return b2Vec2(vector.x, vector.y);
    }

    inline b2Vec2 Tob2Vec2(const Vector3& vector)
    {
        return b2Vec2(vector.x, vector.y);
    }

    inline b2MotionLocks Tob2MotionLocks(const PhysicsConstraintsFlags& constraints)
    {
        b2MotionLocks locks;
        locks.linearX = constraints.Contains(PhysicsConstraintsFlagBits::PositionX);
        locks.linearY = constraints.Contains(PhysicsConstraintsFlagBits::PositionY);
        locks.angularZ = constraints.Contains(PhysicsConstraintsFlagBits::RotationZ);
        return locks;
    }

    inline Vector2 ToVector2(const b2Vec2& vector)
    {
        return Vector2(vector.x, vector.y);
    }

    inline PhysicsConstraintsFlags ToPhysicsConstraints(const b2MotionLocks& motionLocks)
    {
        PhysicsConstraintsFlags constraints;
        if (motionLocks.linearX) constraints |= PhysicsConstraintsFlagBits::PositionX;
        if (motionLocks.linearY) constraints |= PhysicsConstraintsFlagBits::PositionY;
        if (motionLocks.angularZ) constraints |= PhysicsConstraintsFlagBits::RotationZ;
        return constraints;
    }
}
