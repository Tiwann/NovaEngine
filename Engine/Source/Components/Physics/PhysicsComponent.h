#pragma once
#include "Runtime/Component.h"
#include "Physics/PhysicsBodyType.h"
#include "Physics/PhysicsConstraints.h"
#include "Physics/PhysicsMaterial.h"
#include "Containers/MulticastDelegate.h"
#include "Physics/PhysicsShape.h"

namespace Nova
{
    struct Vector3;
    
    class PhysicsComponent : public Component
    {
    public:
        void OnInspectorGUI(const ImGuiIO& IO) override;

        bool GetShowCollisions() const;
        void SetShowCollisions(bool Enabled);

        virtual void SetGravityScale(f32 Scale) = 0;
        virtual void SetLinearVelocity(const Vector3& Velocity) = 0;
        virtual void SetAngularVelocity(const Vector3& AngularVelocity) = 0;
        virtual void SetLinearDamping(f32 LinearDamping) = 0;
        virtual void SetAngularDamping(f32 AngularDamping) = 0;

        virtual f32 GetGravityScale() const = 0;
        virtual Vector3 GetLinearVelocity() const = 0;
        virtual Vector3 GetAngularVelocity() const = 0;
        virtual f32 GetLinearDamping() const = 0;
        virtual f32 GetAngularDamping() const = 0;
        virtual Vector3 GetLinearVelocityPoint(const Vector3& Point) const = 0;

        virtual void AddForce(const Vector3& Force) = 0;
        virtual void AddImpulse(const Vector3& Force) = 0;
        virtual void AddForceAtPosition(const Vector3& Position, const Vector3& Force) = 0;
        virtual void AddImpulseAtPosition(const Vector3& Position, const Vector3& Force) = 0;
        virtual void RenderCollisions(Renderer* Renderer) const = 0;
        
        void OnRender(Renderer* Renderer) override
        {
            if(m_ShowCollisions)
                RenderCollisions(Renderer);
        }
    protected:
        PhysicsComponent(Entity* Owner, const String& Name) : Component(Owner, Name){}
        bool m_ShowCollisions = false;
    };

    template<typename BodyBase, typename ShapeBase, typename CollisionType>
    class IPhysics
    {
    public:
        virtual ~IPhysics() = default;
        using ContactDelegate = MulticastDelegate<void(const CollisionType&)>;
        using TriggerDelegate = MulticastDelegate<void(const CollisionType&)>;
        ContactDelegate OnContactBeginEvent;
        ContactDelegate OnContactStayEvent;
        ContactDelegate OnContactEndEvent;

        TriggerDelegate OnTriggerBeginEvent;
        TriggerDelegate OnTriggerStayEvent;
        TriggerDelegate OnTriggerEndEvent;
    protected:
 
        virtual ShapeBase* CreateShape(Transform* Transform) = 0;
        virtual void RecreatePhysicsState() = 0;
        virtual const PhysicsMaterial& GetMaterial() const = 0;
        virtual void SetMaterial(const PhysicsMaterial& Material) = 0;
        virtual PhysicsConstraintsFlags GetConstraints() = 0;
        virtual void SetConstraints(PhysicsConstraintsFlags Constraints) = 0;
        virtual bool IsSensor() const = 0;
        virtual void SetSensor(bool Sensor) = 0;
        
        virtual PhysicsBodyType GetPhysicsBodyType() const = 0;
        virtual void SetPhysicsBodyType(PhysicsBodyType Type) = 0;
        
        BodyBase* m_PhysicsBody = nullptr;
        ShapeBase* m_PhysicsShape = nullptr;
    };
}
