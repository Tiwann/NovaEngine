#pragma once
#include "Filesystem.h"
#include "LogCategory.h"
#include "Types.h"
#include "Containers/String.h"


struct GLFWwindow;

namespace Nova
{
    class Image;
    struct Vector2;
    struct ApplicationConfiguration;
    
    NOVA_DECLARE_LOG_CATEGORY_STATIC(Input, "INPUT")
    
    class Window
    {
    public:
        explicit Window(const ApplicationConfiguration& Config);
        ~Window();
        
        void Destroy();
        const GLFWwindow* GetNativeWindow() const;
        GLFWwindow* GetNativeWindow();
        
        bool ShouldClose() const;
        bool IsValid() const;

        template<typename T = f32> requires IsArithmeticValue<T>
        T GetWidth() const
        {
            return static_cast<T>(m_Width);
        }

        template<typename T = f32> requires IsArithmeticValue<T>
        T GetHeight() const
        {
            return static_cast<T>(m_Height);
        }

        Vector2 GetSize() const;
        Vector2 GetCenter() const;
        
        const String& GetName() const;
        void SetName(String&& name);
        void ResetName() const;
        void SetIcon(const Path& Filepath)const;
        void SetIcon(const Image* Image) const;
        bool IsResizable() const;
        
        void Show() const;
        void Hide() const;
        bool IsVisible() const { return m_Visible; }
        bool IsMinimized() const { return m_Minimized; }
        bool IsMaximized() const { return m_Maximized; }

    private:
        friend class Application;
        GLFWwindow* m_Handle = nullptr;
        String m_Name;
        u32 m_Width{0}, m_Height{0}, m_PositionX{0}, m_PositionY{0};
        bool m_Resizable{false};
        bool m_HasFocus{false};
        bool m_Maximized{false};
        bool m_Minimized{false};
        bool m_Visible{true};
    };
}
