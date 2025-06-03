#include "Window.h"
#include "DesktopWindow.h"

namespace Nova
{
    Window::Window(Application* Owner) : m_Owner(Owner)
    {
    }

    Window* Window::Create(Application* Owner)
    {
#if defined(NOVA_PLATFORM_WINDOWS) || defined(NOVA_PLATFORM_LINUX) || defined(NOVA_PLATFORM_MACOS)
        return new DesktopWindow(Owner);
#elif defined(NOVA_PLATFORM_SWITCH)
        return nullptr;
#elif defined(NOVA_PLATYFORM_PS5)
        return nullptr;
#elif defined(NOVA_PLATFORM_XBOX)
        return nullptr;
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

    Application* Window::GetOwner() const
    {
        return m_Owner;
    }

    u32 Window::GetWidth() const
    {
        return m_Width;
    }

    u32 Window::GetHeight() const
    {
        return m_Height;
    }
}
