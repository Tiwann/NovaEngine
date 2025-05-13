#pragma once
#include "Math/Vector3.h"
#include "PhysicsMaterial.h"
#include "PhysicsBodyType.h"
#include "PhysicsConstraints.h"
#include "Containers/MulticastDelegate.h"

namespace Nova
{
    template<typename ShapeBase, typename WorldBase, typename BodyHandle, typename ContactInfoBase>
    class PhysicsBody
    {
    public:
        using ContactDelegate = MulticastDelegate<void(const ContactInfoBase&)>;
        friend WorldBase;
        
        explicit PhysicsBody(BodyHandle* Handle, WorldBase& World) : m_Handle(Handle), m_World(&World){ }
        virtual ~PhysicsBody() = default;
        PhysicsBody(const PhysicsBody&) = delete;
        PhysicsBody(PhysicsBody&&) noexcept = delete;
        PhysicsBody& operator=(const PhysicsBody&) = delete;
        PhysicsBody& operator=(PhysicsBody&&) noexcept = delete;

        ContactDelegate OnContactBeginEvent;
        ContactDelegate OnContactStayEvent;
        ContactDelegate OnContactEndEvent;
        
        virtual void CreatePhysicsState(ShapeBase* Shape, const PhysicsMaterial& Material)
        {
            m_Shape = Shape;
            m_Material = Material;
        }
        
        virtual void DestroyPhysicsState() = 0;
        
        virtual void SetUserPointer(void* User) = 0;
        virtual void* GetUserPointer() const = 0;

        virtual void SetPosition(const Vector3& Position) = 0;
        virtual Vector3 GetPosition() const = 0;
        virtual void SetRotation(const Quaternion& Rotation) = 0;
        virtual Vector3 GetRotation() const = 0;
        virtual void SetPositionAndRotation(const Vector3& Position, const Vector3& Rotation) = 0;
        
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

        
        virtual const PhysicsConstraintsFlags& GetConstraints() const = 0;
        virtual void SetConstraints(const PhysicsConstraintsFlags& constraints) = 0;
        virtual const PhysicsMaterial& GetMaterial() const = 0;
        virtual void SetMaterial(const PhysicsMaterial& Material) = 0;
        virtual PhysicsBodyType GetType() const = 0;
        virtual void SetType(PhysicsBodyType Type) = 0;
        virtual bool IsSensor() = 0;
        virtual void SetIsSensor(bool Sensor) = 0;


        bool IsColliding() const { return m_IsColliding; }

        BodyHandle* GetHandle() { return m_Handle; }
        const BodyHandle* GetHandle() const { return m_Handle; }

        WorldBase* GetWorld() { return m_World; }
        const WorldBase* GetWorld() const { return m_World; }

        ShapeBase* GetShape() { return m_Shape; }
        const ShapeBase* GetShape() const { return m_Shape; }
    
    protected:
        PhysicsConstraintsFlags m_Constraints;
        PhysicsMaterial m_Material;
        PhysicsBodyType m_Type = PhysicsBodyType::Static;
        bool m_IsSensor = false;
        bool m_IsColliding = false;
    private:
        BodyHandle* m_Handle = nullptr;
        WorldBase* m_World = nullptr;
        ShapeBase* m_Shape = nullptr;
    };
}
