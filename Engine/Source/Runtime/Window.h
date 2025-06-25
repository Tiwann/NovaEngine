#pragma once
#include "Object.h"
#include "Containers/String.h"
#include "LogCategory.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(Window, "WINDOW")

namespace Nova
{
    struct WindowCreateInfo
    {
        String Title;
        u32 Width;
        u32 Height;
        bool Resizable;
        bool Show;
    };

    class Application;

    class Window : public Object
    {
    public:
        explicit Window(Application* Owner);
        ~Window() override = default;
        static Window* Create(Application* Owner);

        virtual bool Initialize(const WindowCreateInfo& CreateInfo) = 0;
        virtual void Update(f32 Delta) = 0;
        virtual void Destroy() = 0;

        void SetTitle(const String& Title);
        const String& GetTitle() const;
        Application* GetOwner() const;
        u32 GetWidth() const;
        u32 GetHeight() const;
    protected:
        Application* m_Owner = nullptr;
        String m_Title;
        u32 m_Width = 0, m_Height = 0;
    };
}
