#pragma once

namespace Nova
{
    template<typename WorldType, typename BodyType, typename ContactType>
    struct PhysicsContact
    {
        WorldType* World = nullptr;
        BodyType* BodyA = nullptr;
        BodyType* BodyB = nullptr;
        ContactType* Handle = nullptr;

        PhysicsContact(WorldType* InWorld, BodyType* InBodyA, BodyType* InBodyB, ContactType* InHandle) : World(InWorld), BodyA(InBodyA), BodyB(InBodyB), Handle(InHandle) {}
    };
}