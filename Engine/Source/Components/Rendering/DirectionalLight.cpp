#include "DirectionalLight.h"

namespace Nova
{
    DirectionalLight::DirectionalLight(Entity* Owner)
        : LightComponent(Owner, "Directional Light")
    {
        m_Type = LightType::Directional;
    }
}
