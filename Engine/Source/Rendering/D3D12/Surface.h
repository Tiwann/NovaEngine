#pragma once
#include "Rendering/Surface.h"

namespace Nova
{
    class Window;
}

namespace Nova::D3D12
{
    class Surface final : public Nova::Surface
    {
    public:
        bool Initialize(const SurfaceCreateInfo& createInfo) override;
        void Destroy() override;
    };
}
