#pragma once
#include <CoreExport.h>

namespace Nova
{
    struct CORE_API GraphicsSettings
    {
        enum class SwapchainBufferType
        {
            DoubleBuffering = 2,
            TripleBuffering = 3
        };
        
        SwapchainBufferType BufferType = SwapchainBufferType::DoubleBuffering;
        bool                VSync      = true;
         
        static GraphicsSettings DefaultSettings;
    };
}
