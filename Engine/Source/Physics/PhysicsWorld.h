#pragma once
#include "PhysicsBodyDefinition.h"
#include "Containers/Array.h"

namespace Nova
{
    struct PhysicsMaterial;
    class Scene;
    
    template<typename BodyType, typename ShapeType, typename ContactType>
    class PhysicsWorld
    {
    public:
        using Base = PhysicsWorld;
        using BodyArray = Array<BodyType*>;
        virtual ~PhysicsWorld() = default;
        virtual void OnInit(Scene* Owner) = 0;
        virtual void Step(f32 TimeStep) = 0;
        virtual void OnDestroy() = 0;
        
        virtual void OnContactBegin(const ContactType* Contact) { }
        virtual void OnContactStay(const ContactType* Contact) { }
        virtual void OnContactEnd(const ContactType* Contact) { }
        
        virtual BodyType* CreateBody(const PhysicsBodyDefinition& Definition, const PhysicsMaterial& Material, ShapeType* Shape) = 0;
        virtual void DestroyBody(BodyType* Body) = 0;
        virtual void SetMaterial(BodyType* Body, const PhysicsMaterial& Material) = 0;

        // User can't modify this array neither modify the bodies contained in it
        const BodyArray& GetBodies() const { return m_Bodies; }
    protected:
        BodyArray m_Bodies;
        Scene* m_Owner = nullptr;
    };
    
}
