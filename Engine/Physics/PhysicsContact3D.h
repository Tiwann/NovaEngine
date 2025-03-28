#pragma once
#include "PhysicsContact.h"
#include <PhysicsExport.h>

namespace JPH { struct Contact; }

namespace Nova
{
    class PhysicsWorld3D;
    class PhysicsBody3D;
    
    struct PHYSICS_API PhysicsContact3D : PhysicsContact<PhysicsWorld3D, PhysicsBody3D, JPH::Contact>
    {
        PhysicsContact3D(PhysicsWorld3D* InWorld, PhysicsBody3D* InBodyA, PhysicsBody3D* InBodyB, JPH::Contact* InHandle)
            : PhysicsContact(InWorld, InBodyA, InBodyB, InHandle) {}
    };
}
