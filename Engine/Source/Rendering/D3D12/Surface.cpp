#include "Surface.h"

namespace Nova::D3D12
{
    bool Surface::Initialize(const SurfaceCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (!createInfo.window) return false;

        m_Device = createInfo.device;
        m_Window = createInfo.window;
        return false;
    }

    void Surface::Destroy()
    {
        m_Device = nullptr;
        m_Window = nullptr;
    }
}