#pragma once
#include "GraphicsApi.h"
#include "SwpchainBuffering.h"

namespace Nova
{
    struct GraphicsSettings
    {
        GraphicsApi GraphicsApi;
        SwapchainBuffering Buffering;
        bool VSync;
    };
}
