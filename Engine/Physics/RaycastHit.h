#pragma once
#include "Components/Physics/RigidBody3D.h"
#include "Components/Physics/RigidBody2D.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

namespace Nova
{
    template<typename Vector, typename Component>
    struct RaycastHit
    {
        Vector Point;
        Vector Normal;
        Vector Location;
        f32 Fraction;
        f32 Distance;
        Ref<Component> HitComponent;
    };

    using RaycastHit2D = RaycastHit<Vector2, RigidBody2D>;
    using RaycastHit3D = RaycastHit<Vector3, RigidBody3D>;
}
