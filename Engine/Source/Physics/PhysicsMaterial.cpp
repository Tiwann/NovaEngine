#pragma once
#include "PhysicsMaterial.h"

namespace Nova
{
    String PhysicsMaterial::GetAssetType() const
    {
        return "Physics Material";
    }

    float PhysicsMaterial::GetDensity() const
    {
        return m_Density;
    }

    void PhysicsMaterial::SetDensity(const float density)
    {
        m_Density = density;
    }

    float PhysicsMaterial::GetFriction() const
    {
        return m_Friction;
    }

    void PhysicsMaterial::SetFriction(const float friction)
    {
        m_Friction = friction;
    }

    float PhysicsMaterial::GetBounciness() const
    {
        return m_Bounciness;
    }

    void PhysicsMaterial::SetBounciness(const float bounciness)
    {
        m_Bounciness = bounciness;
    }
}
