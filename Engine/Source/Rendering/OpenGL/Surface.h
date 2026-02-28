#pragma once
#include "Rendering/Surface.h"

namespace Nova::OpenGL
{
    class RenderDevice;

    class Surface final : public Nova::Surface
    {
    public:
        bool Initialize(const SurfaceCreateInfo& createInfo) override;
        void Destroy() override;
    };
}
