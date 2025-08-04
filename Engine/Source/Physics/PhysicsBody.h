#pragma once
#include "PhysicsMaterial.h"
#include "PhysicsBodyType.h"
#include "PhysicsConstraints.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

namespace Nova
{
    class PhysicsWorld;

    class PhysicsBody
    {
    public:
        explicit PhysicsBody(const PhysicsWorld& world) : m_World(&world){ }
        virtual ~PhysicsBody() = default;
        
        void SetUserPointer(void* user) { m_UserPointer = user; }
        void* GetUserPointer() const { return m_UserPointer; }

        virtual void SetPosition(const Vector3& position) = 0;
        virtual Vector3 GetPosition() const = 0;
        virtual void SetRotation(const Quaternion& rotation) = 0;
        virtual Quaternion GetRotation() const = 0;
        virtual void SetPositionAndRotation(const Vector3& position, const Quaternion& rotation) = 0;

        virtual void SetLinearVelocity(const Vector3& velocity) = 0;
        virtual Vector3 GetLinearVelocity() const = 0;

        virtual void SetAngularVelocity(const Vector3& angularVelocity) = 0;
        virtual Vector3 GetAngularVelocity() const = 0;

        virtual void SetLinearDamping(float linearDamping) = 0;
        virtual float GetLinearDamping() const = 0;
        virtual void SetAngularDamping(float angularDamping) = 0;
        virtual float GetAngularDamping() const = 0;

        virtual Vector3 GetLinearVelocityPoint(const Vector3& point) const = 0;

        virtual void AddForce(const Vector3& force) = 0;
        virtual void AddImpulse(const Vector3& force) = 0;
        virtual void AddForceAtPosition(const Vector3& position, const Vector3& force) = 0;
        virtual void AddImpulseAtPosition(const Vector3& position, const Vector3& force) = 0;

        virtual const PhysicsConstraintsFlags& GetConstraints() const = 0;
        virtual void SetConstraints(const PhysicsConstraintsFlags& constraints) = 0;

        virtual const PhysicsMaterial& GetMaterial() const = 0;
        virtual void SetMaterial(const PhysicsMaterial& material) = 0;

        virtual PhysicsBodyType GetType() const = 0;
        virtual void SetType(PhysicsBodyType type) = 0;

        virtual bool IsSensor() = 0;
        virtual void SetIsSensor(bool isSensor) = 0;

        bool IsColliding() const { return m_IsColliding; }

        const PhysicsWorld* GetWorld() const { return m_World; }
    protected:
        PhysicsConstraintsFlags m_Constraints;
        PhysicsMaterial m_Material;
        PhysicsBodyType m_Type = PhysicsBodyType::Static;
        bool m_IsSensor = false;
        bool m_IsColliding = false;
    private:
        const PhysicsWorld* m_World = nullptr;
        void* m_UserPointer = nullptr;
    };
}
