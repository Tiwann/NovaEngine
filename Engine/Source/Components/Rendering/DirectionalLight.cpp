#include "DirectionalLight.h"

namespace Nova
{
    DirectionalLight::DirectionalLight(Entity* owner) : LightComponent(owner, "Directional Light")
    {
        m_Type = LightType::Directional;
    }
}
