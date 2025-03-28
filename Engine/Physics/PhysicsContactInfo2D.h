#pragma once
#include "PhysicsContactInfo.h"
#include "Math/Vector2.h"
#include <PhysicsExport.h>

namespace Nova
{
    class PhysicsBody2D;
    struct PHYSICS_API PhysicsContactInfo2D : PhysicsContactInfo<PhysicsBody2D, Vector2>{};
}