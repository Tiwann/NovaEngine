#pragma once
#include "LightComponent.h"

namespace Nova
{
    class SpotLight : public LightComponent
    {
    public:
        SpotLight(Entity* Owner);

        f32 GetRadius() const;
        void SetRadius(f32 Radius);
    private:
        f32 m_Radius = 1.0f;
    };
}
