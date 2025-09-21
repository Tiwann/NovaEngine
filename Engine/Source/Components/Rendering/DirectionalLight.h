#pragma once
#include "LightComponent.h"

namespace Nova
{
    class DirectionalLight final : public LightComponent
    {
    public:
        explicit DirectionalLight(Entity* owner);
    };
}
