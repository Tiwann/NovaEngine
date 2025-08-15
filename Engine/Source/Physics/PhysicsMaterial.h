#pragma once
#include "Runtime/Asset.h"

namespace Nova
{
    class PhysicsMaterial : public Asset
    {
    public:
        PhysicsMaterial() : Asset("Physics Material"){}
        ~PhysicsMaterial() override = default;

        String GetAssetType() const override;

        float GetDensity() const;
        void SetDensity(float density);
        float GetFriction() const;
        void SetFriction(float friction);
        float GetBounciness() const;
        void SetBounciness(float bounciness);
    private:
        float m_Density = 1.0f;
        float m_Friction = 0.0f;
        float m_Bounciness = 0.0f;
    };
}
