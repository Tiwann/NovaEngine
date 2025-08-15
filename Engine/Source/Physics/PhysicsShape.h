#pragma once
#include "Runtime/Ref.h"

namespace Nova
{
    class PhysicsMaterial;
    class PhysicsBody;

    class PhysicsShape
    {
    public:
        PhysicsShape() = default;
        virtual ~PhysicsShape() = default;

        virtual void SetPhysicsMaterial(const Ref<PhysicsMaterial>& material) { m_Material = material; }
        virtual Ref<PhysicsMaterial> GetPhysicsMaterial() const { return m_Material; }
        virtual void SetIsSensor(const bool isSensor) { m_IsSensor = isSensor; }
        virtual bool IsSensor() const { return m_IsSensor; }
    protected:
        Ref<PhysicsMaterial> m_Material = nullptr;
        bool m_IsSensor = false;
    };
}
