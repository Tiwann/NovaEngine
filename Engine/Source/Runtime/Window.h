#pragma once
#include "Object.h"
#include "Containers/String.h"
#include "Containers/MulticastDelegate.h"
#include "Ref.h"
#include "Flags.h"
#include "Math/Rect.h"
#include <cstdint>


namespace Nova
{
    enum class WindowCreateFlagBits
    {
        None = 0,
        Resizable = BIT(0),
        Decorated = BIT(1),
        Fullscreen = BIT(2),
        CreateAtCenter = BIT(3),
        Show = BIT(4),

        Default = Show | Decorated,
    };

    typedef Flags<WindowCreateFlagBits> WindowCreateFlags;

    struct WindowCreateInfo
    {
        String title;
        uint32_t width;
        uint32_t height;
        WindowCreateFlags flags;
    };

    class Window : public Object
    {
    protected:
        using CloseDelegate = MulticastDelegate<void()>;
        using ResizeDelegate = MulticastDelegate<void(uint32_t, uint32_t)>;
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
        Rect2D<uint32_t> GetBounds() const;


        CloseDelegate CloseEvent;
        ResizeDelegate ResizeEvent;
        MinimizeDelegate MinimizeEvent;
        MaximizeDelegate MaximizeEvent;
    protected:
        String m_Title;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
    };

    Ref<Window> CreateWindow(const WindowCreateInfo& createInfo);
}
