#include "AmbientLight.h"

namespace Nova
{
    AmbientLight::AmbientLight(Entity* Owner): LightComponent(Owner, "Ambient Light")
    {
        m_Type = LightType::Ambient;
    }

}