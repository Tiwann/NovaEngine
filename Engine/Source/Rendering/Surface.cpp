#include "Surface.h"
#include "Runtime/DesktopWindow.h"
#include "Runtime/Window.h"
#include <GLFW/glfw3.h>
namespace Nova
{
    RenderDevice* Surface::GetDevice() const
    {
        return m_Device;
    }

    Window* Surface::GetWindow() const
    {
        return m_Window;
    }

    uint32_t Surface::GetWidth() const
    {
        return m_Window->GetWidth();
    }

    uint32_t Surface::GetHeight() const
    {
        return m_Window->GetHeight();
    }

    bool Surface::IsAvailable() const
    {
        if (const DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(m_Window))
        {
            if (desktopWindow->IsMinimized())
                return false;
        }

        return true;
    }

    void Surface::SwapBuffers() const
    {
        if (DesktopWindow* desktopWindow = dynamic_cast<DesktopWindow*>(m_Window))
        {
            glfwSwapBuffers(desktopWindow->GetHandle());
        }
    }
}
