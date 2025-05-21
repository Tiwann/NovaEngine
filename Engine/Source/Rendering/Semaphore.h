#pragma once
#include "RendererObject.h"

namespace Nova
{
    struct SemaphoreCreateInfo
    {

    };

    class Semaphore : public RendererObject<SemaphoreCreateInfo>
    {
    public:
        explicit Semaphore(Renderer* Owner);
    };
}
