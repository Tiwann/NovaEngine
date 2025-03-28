#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <PhysicsExport.h>

namespace JPH
{
    struct PHYSICS_API Contact
    {
        const Body* BodyA;
        const Body* BodyB;
        const ContactManifold* Manifold;
        const ContactSettings* Settings;
    };
}