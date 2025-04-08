#pragma once

namespace Nova
{
    struct GraphicsSettings
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
