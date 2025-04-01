#pragma once
#include "PhysicsContactInfo.h"
#include "Math/Vector2.h"
#include <CoreExport.h>

namespace Nova
{
    class PhysicsBody2D;
    struct CORE_API PhysicsContactInfo2D : PhysicsContactInfo<PhysicsBody2D, Vector2>{};
}