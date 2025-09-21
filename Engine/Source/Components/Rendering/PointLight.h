#pragma once
#include "LightComponent.h"

namespace Nova
{
    class PointLight final : public LightComponent
    {
    public:
        explicit PointLight(Entity* owner);

        float GetInnerRadius() const;
        void SetInnerRadius(float innerRadius);
        
        float GetOuterRadius() const;
        void SetOuterRadius(float outerRadius);

    private:
        float m_InnerRadius = 1.0f;
        float m_OuterRadius = 1.5f;
    };
}
