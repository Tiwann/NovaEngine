#include "PointLight.h"

namespace Nova
{
    PointLight::PointLight(Entity* owner) : LightComponent(owner, "Point Light")
    {
        m_Type = LightType::Point;
    }

    float PointLight::GetInnerRadius() const
    {
        return m_InnerRadius;
    }

    void PointLight::SetInnerRadius(const float innerRadius)
    {
        m_InnerRadius = innerRadius;
    }

    float PointLight::GetOuterRadius() const
    {
        return m_OuterRadius;
    }

    void PointLight::SetOuterRadius(const float outerRadius)
    {
        m_OuterRadius = outerRadius;
    }
}
