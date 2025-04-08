#pragma once
#include "LightComponent.h"

namespace Nova
{
    class AmbientLight : public LightComponent
    {
    public:
        AmbientLight(Entity* Owner) : LightComponent(Owner, "Ambient Light")
        {
            m_Type = LightType::Ambient;
        }
    private:
    };
}
