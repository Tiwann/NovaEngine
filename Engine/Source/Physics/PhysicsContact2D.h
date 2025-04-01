#pragma once
#include "PhysicsContact.h"
#include <CoreExport.h>

class b2Contact;

namespace Nova
{
    class PhysicsWorld2D;
    class PhysicsBody2D;
    
    struct CORE_API PhysicsContact2D : PhysicsContact<PhysicsWorld2D, PhysicsBody2D, b2Contact>
    {
        PhysicsContact2D(PhysicsWorld2D* InWorld, PhysicsBody2D* InBodyA, PhysicsBody2D* InBodyB, b2Contact* InContact)
            : PhysicsContact(InWorld, InBodyA, InBodyB, InContact) {}
    };
}
