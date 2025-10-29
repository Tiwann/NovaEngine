#include "Window.h"
#include "DesktopWindow.h"


namespace Nova
{
    Ref<Window> CreateWindow(const WindowCreateInfo& createInfo)
    {
#if defined(NOVA_PLATFORM_WINDOWS) || defined(NOVA_PLATFORM_LINUX)
        Window* window = new DesktopWindow();
        if (!window->Initialize(createInfo))
        {
            delete window;
            window = nullptr;
        }
        return Ref(window);
#endif
    }

    void Window::SetTitle(const String& title)
    {
        m_Title = title;
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
