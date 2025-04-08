#pragma once
#include "LightComponent.h"

namespace Nova
{
    class PointLight : public LightComponent
    {
    public:
        PointLight(Entity* Owner);

        f32 GetInnerRadius() const;
        void SetInnerRadius(f32 InnerRadius);
        
        f32 GetOuterRadius() const;
        void SetOuterRadius(f32 OuterRadius);

    private:
        f32 m_InnerRadius = 1.0f;
        f32 m_OuterRadius = 1.5f;
    };
}
