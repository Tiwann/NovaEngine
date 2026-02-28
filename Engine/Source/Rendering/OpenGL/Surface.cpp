#include "Surface.h"
#include "RenderDevice.h"

namespace Nova::OpenGL
{
    bool Surface::Initialize(const SurfaceCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (!createInfo.window) return false;
        m_Device = static_cast<RenderDevice*>(createInfo.device);
        m_Window = createInfo.window;
        return true;
    }

    void Surface::Destroy()
    {
        m_Device = nullptr;
        m_Window = nullptr;
    }
}
