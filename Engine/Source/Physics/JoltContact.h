#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <CoreExport.h>

namespace JPH
{
    struct CORE_API Contact
    {
        const Body* BodyA;
        const Body* BodyB;
        const ContactManifold* Manifold;
        const ContactSettings* Settings;
    };
}