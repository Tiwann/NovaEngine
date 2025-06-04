#pragma once
#include "RendererObject.h"


namespace Nova
{
    class Window;

    struct SurfaceCreateInfo
    {
        Window* Window = nullptr;
    };

    class Surface : public RendererObject<SurfaceCreateInfo>
    {
    public:
        explicit Surface(Renderer* Owner) : RendererObject("Surface", Owner) {}
    };
}
