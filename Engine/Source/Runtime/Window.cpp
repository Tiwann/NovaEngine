#include "Window.h"

namespace Nova
{
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
