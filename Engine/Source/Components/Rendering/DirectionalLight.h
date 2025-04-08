#pragma once
#include "LightComponent.h"

namespace Nova
{
    class DirectionalLight : public LightComponent
    {
    public:
        DirectionalLight(Entity* Owner);
    };
}
