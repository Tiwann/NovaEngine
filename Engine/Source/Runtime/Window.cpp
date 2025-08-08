#include "Window.h"

#include "DesktopWindow.h"

namespace Nova
{
    Window* Window::Create(const WindowCreateInfo& createInfo)
    {
#if defined(NOVA_PLATFORM_WINDOWS) || defined(NOVA_PLATFORM_LINUX)
        Window* window = new DesktopWindow();
        if (!window->Initialize(createInfo))
        {
            delete window;
            window = nullptr;
        }
        return window;
#endif
    }

    void Window::SetTitle(const String& Title)
    {
        m_Title = Title;
    }

    const String& Window::GetTitle() const
    {
        return m_Title;
    }

    uint32_t Window::GetWidth() const
    {
        return m_Width;
    }

    uint32_t Window::GetHeight() const
    {
        return m_Height;
    }
}
