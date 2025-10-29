#pragma once
#include "LightComponent.h"

namespace Nova
{
    class AmbientLight final : public LightComponent
    {
    public:
        explicit AmbientLight(Entity* Owner);
    };
}
