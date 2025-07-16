#pragma once
#include "Object.h"
#include "Containers/String.h"
#include "Containers/MulticastDelegate.h"
#include <cstdint>

namespace Nova
{
    struct WindowCreateInfo
    {
        String title;
        uint32_t width;
        uint32_t height;
        bool resizable;
        bool show;
    };

    class Window : public Object
    {
    protected:
        using CloseDelegate = MulticastDelegate<void()>;
        using ResizeDelegate = MulticastDelegate<void(int32_t, int32_t)>;
        using MinimizeDelegate = MulticastDelegate<void()>;
        using MaximizeDelegate = MulticastDelegate<void()>;
    public:
        Window() = default;
        ~Window() override = default;

        virtual bool Initialize(const WindowCreateInfo& createInfo) = 0;
        virtual void PollEvents() = 0;
        virtual void Destroy() = 0;

        void SetTitle(const String& title);
        const String& GetTitle() const;
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;

        CloseDelegate closeEvent;
        ResizeDelegate resizeEvent;
        MinimizeDelegate minimizeEvent;
        MaximizeDelegate maximizeEvent;
    protected:
        String m_Title;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
    };
}
