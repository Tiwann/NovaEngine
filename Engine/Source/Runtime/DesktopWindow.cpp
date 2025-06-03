#include "DesktopWindow.h"
#include "Application.h"
#include "ExitCode.h"
#include "Rendering/Renderer.h"

#include <GLFW/glfw3.h>

namespace Nova
{
    DesktopWindow::DesktopWindow(Application* Owner) : Window(Owner)
    {
    }

    bool DesktopWindow::Initialize(const WindowCreateInfo& CreateInfo)
    {
        glfwWindowHint(GLFW_RESIZABLE, CreateInfo.Resizable);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Handle = glfwCreateWindow((int)m_Width, (int)m_Height, *CreateInfo.Title, nullptr, nullptr);
        if (!m_Handle)
            return false;

        glfwSetWindowUserPointer(m_Handle, this);

        glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* Handle)
        {
            const DesktopWindow* Window = (DesktopWindow*)glfwGetWindowUserPointer(Handle);
            Application* Owner = Window->GetOwner();
            Owner->RequireExit(ExitCode::Success);
        });

        glfwSetWindowFocusCallback(m_Handle, [](GLFWwindow* Handle, const int Focus){
            DesktopWindow* Window = (DesktopWindow*)glfwGetWindowUserPointer(Handle);
            Window->m_HasFocus = (bool)Focus;
        });

        glfwSetWindowMaximizeCallback(m_Handle, [](GLFWwindow* Handle, const int Maximized){
            DesktopWindow* Window = (DesktopWindow*)glfwGetWindowUserPointer(Handle);
            Window->m_IsMaximized = (bool)Maximized;

            const Application* Owner = Window->GetOwner();
            Renderer* Renderer = Owner->GetRenderer();
            Renderer->ShouldRecreateSwapchain = true;
        });

        glfwSetWindowPosCallback(m_Handle, [](GLFWwindow* Handle, const int NewX, const int NewY){
            DesktopWindow* Window = (DesktopWindow*)glfwGetWindowUserPointer(Handle);
            Window->m_PosX = NewX;
            Window->m_PosY = NewY;
        });

        glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow* Handle, const int NewWidth, const int NewHeight){
            DesktopWindow* Window = (DesktopWindow*)glfwGetWindowUserPointer(Handle);
            Window->m_Width = NewWidth;
            Window->m_Height = NewHeight;

            const Application* Owner = Window->GetOwner();
            Renderer* Renderer = Owner->GetRenderer();
            Renderer->ShouldRecreateSwapchain = true;
        });

        glfwSetWindowIconifyCallback(m_Handle, [](GLFWwindow* Handle, const int Iconified)
        {
            DesktopWindow* Window = (DesktopWindow*)glfwGetWindowUserPointer(Handle);
            Window->m_IsMinimized = (bool)Iconified;
        });

        glfwSetKeyCallback(m_Handle, [](GLFWwindow* Handle, int Key, int, const int Action, int)
        {
            DesktopWindow* Window = (DesktopWindow*)glfwGetWindowUserPointer(Handle);
            switch (Action)
            {
            case GLFW_PRESS:
                Window->m_KeyStates[(KeyCode)Key] = InputState::Pressed;
                break;
            case GLFW_RELEASE:
                Window->m_KeyStates[(KeyCode)Key] = InputState::Released;
                break;
            default: break;
            }
        });

        glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* Handle, int Button, const int Action, int)
        {
            DesktopWindow* Window = (DesktopWindow*)glfwGetWindowUserPointer(Handle);
            switch (Action)
            {
            case GLFW_PRESS:
                Window->m_MouseButtonStates[(MouseButton)Button] = InputState::Pressed;
                break;
            case GLFW_RELEASE:
                Window->m_MouseButtonStates[(MouseButton)Button] = InputState::Released;
                break;
            default: break;
            }
        });

        return true;
    }

    void DesktopWindow::Update(f32 Delta)
    {
        for(auto& [Key, State] : m_KeyStates)
        {
            State = InputState::None;
        }

        for(auto& [MouseButton, State] : m_MouseButtonStates)
        {
            State = InputState::None;
        }

        for(auto& Gamepad : m_GamepadButtons)
        {
            for(size_t i = 0; i < 16; ++i)
                Gamepad[i] = InputState::None;
        }

        m_LastGamepadStates = m_GamepadStates;

        for(size_t ID = 0; ID < 16; ++ID)
        {
            glfwGetGamepadState((int)ID, (GLFWgamepadstate*)&m_GamepadStates[ID]);

            for(size_t ButtonIndex = 0 ; ButtonIndex < 15; ++ButtonIndex)
            {
                if(m_GamepadStates[ID].Buttons[ButtonIndex] != m_LastGamepadStates[ID].Buttons[ButtonIndex])
                {
                    const InputState State = m_GamepadStates[ID].Buttons[ButtonIndex] ? InputState::Pressed : InputState::Released;
                    m_GamepadButtons[ID][ButtonIndex] = State;
                }
            }

            for(size_t Axis = 0 ; Axis < 6; ++Axis)
            {
                m_GamepadAxes[ID][Axis] = m_GamepadStates[ID].Axes[Axis];
            }
            
            m_LastGamepadStates[ID] = m_GamepadStates[ID];
        }


        m_MousePosition = GetMousePosition();
        m_DeltaMousePosition = m_LastMousePosition - m_MousePosition;
        m_LastMousePosition = m_MousePosition;

        glfwPollEvents();
    }

    void DesktopWindow::Destroy()
    {
        glfwDestroyWindow(m_Handle);
    }


    GLFWwindow* DesktopWindow::GetHandle()
    {
        return m_Handle;
    }

    const GLFWwindow* DesktopWindow::GetHandle() const
    {
        return m_Handle;
    }

    void DesktopWindow::Show()
    {
        glfwShowWindow(m_Handle);
    }

    void DesktopWindow::Hide()
    {
        glfwHideWindow(m_Handle);
    }

    void DesktopWindow::SetPosition(const i32 NewPosX, const i32 NewPosY)
    {
        glfwSetWindowPos(m_Handle, NewPosX, NewPosY);
    }

    bool DesktopWindow::HasFocus() const
    {
        return m_HasFocus;
    }

    bool DesktopWindow::IsMinimized() const
    {
        return m_IsMinimized;
    }

    bool DesktopWindow::IsMaximized() const
    {
        return m_IsMaximized;
    }

    void DesktopWindow::SetCursorHidden()
    {
        glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    Vector2 DesktopWindow::GetMousePosition() const
    {
        f64 XPos = 0, YPos = 0;
        glfwGetCursorPos(m_Handle, &XPos, &YPos);
        return Vector2(XPos, YPos);
    }

    Vector2 DesktopWindow::GetDeltaMousePosition() const
    {
        return m_DeltaMousePosition;
    }

    bool DesktopWindow::GetKeyDown(const KeyCode KeyCode) const
    {
        return m_KeyStates[KeyCode] == InputState::Pressed;
    }

    bool DesktopWindow::GetKey(KeyCode KeyCode) const
    {
        const int State = glfwGetKey(m_Handle, (int)KeyCode);
        return State == GLFW_PRESS;
    }

    bool DesktopWindow::GetKeyUp(const KeyCode KeyCode) const
    {
        return m_KeyStates[KeyCode] == InputState::Released;
    }

    bool DesktopWindow::GetMouseButtonDown(const MouseButton MouseButton) const
    {
        return m_MouseButtonStates[MouseButton] == InputState::Pressed;
    }

    bool DesktopWindow::GetMouseButton(MouseButton MouseButton) const
    {
        const int State = glfwGetMouseButton(m_Handle, (int)MouseButton);
        return State == GLFW_PRESS;
    }

    bool DesktopWindow::GetMouseButtonUp(const MouseButton MouseButton) const
    {
        return m_MouseButtonStates[MouseButton] == InputState::Released;
    }
}
