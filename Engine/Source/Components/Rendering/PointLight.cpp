#include "PointLight.h"

namespace Nova
{
    PointLight::PointLight(Entity* Owner) : LightComponent(Owner, "Point Light")
    {
        m_Type = LightType::Point;
    }

    f32 PointLight::GetInnerRadius() const
    {
        return m_InnerRadius;
    }

    void PointLight::SetInnerRadius(const f32 InnerRadius)
    {
        m_InnerRadius = InnerRadius;
    }

    f32 PointLight::GetOuterRadius() const
    {
        return m_OuterRadius;
    }

    void PointLight::SetOuterRadius(const f32 OuterRadius)
    {
        m_OuterRadius = OuterRadius;
    }
}
