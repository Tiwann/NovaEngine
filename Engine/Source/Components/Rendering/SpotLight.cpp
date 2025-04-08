#include "SpotLight.h"

namespace Nova
{
    SpotLight::SpotLight(Entity* Owner) : LightComponent(Owner, "Spot Light")
    {
        m_Type = LightType::Spot;
    }

    f32 SpotLight::GetRadius() const
    {
        return m_Radius;
    }

    void SpotLight::SetRadius(f32 Radius)
    {
        m_Radius = Radius;
    }
}
