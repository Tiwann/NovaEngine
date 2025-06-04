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
        if (m_Handle)
        {
            glfwDestroyWindow(m_Handle);
            m_Handle = nullptr;
        }

        glfwWindowHint(GLFW_RESIZABLE, CreateInfo.Resizable);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Handle = glfwCreateWindow(CreateInfo.Width, CreateInfo.Height, *CreateInfo.Title, nullptr, nullptr);
        if (!m_Handle)
            return false;

        m_Width = CreateInfo.Width;
        m_Height = CreateInfo.Height;
        m_Title = CreateInfo.Title;

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
            for(size_t i = 0; i < GAMEPAD_BUTTON_COUNT; ++i)
                Gamepad[i] = InputState::None;
        }

        m_LastGamepadStates = m_GamepadStates;

        for(size_t ID = 0; ID < GAMEPAD_COUNT; ++ID)
        {
            glfwGetGamepadState((int)ID, (GLFWgamepadstate*)&m_GamepadStates[ID]);

            for(size_t ButtonIndex = 0 ; ButtonIndex < GAMEPAD_BUTTON_COUNT; ++ButtonIndex)
            {
                if(m_GamepadStates[ID].Buttons[ButtonIndex] != m_LastGamepadStates[ID].Buttons[ButtonIndex])
                {
                    const InputState State = m_GamepadStates[ID].Buttons[ButtonIndex] ? InputState::Pressed : InputState::Released;
                    m_GamepadButtons[ID][ButtonIndex] = State;
                }
            }

            for(size_t Axis = 0 ; Axis < GAMEPAD_AXIS_COUNT; ++Axis)
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

    void DesktopWindow::SetCursorMode(const CursorMode& Mode) const
    {
        switch (Mode)
        {
        case CursorMode::Normal:
            glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case CursorMode::Hidden:
            glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
        case CursorMode::Locked:
            glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        }
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

    bool DesktopWindow::GetKeyDown(const KeyCode& KeyCode) const
    {
        return m_KeyStates.Contains(KeyCode) && m_KeyStates[KeyCode] == InputState::Pressed;
    }

    bool DesktopWindow::GetKey(const KeyCode& KeyCode) const
    {
        const int State = glfwGetKey(m_Handle, (int)KeyCode);
        return State == GLFW_PRESS;
    }

    bool DesktopWindow::GetKeyUp(const KeyCode& KeyCode) const
    {
        return m_KeyStates.Contains(KeyCode) && m_KeyStates[KeyCode] == InputState::Released;
    }

    bool DesktopWindow::GetMouseButtonDown(const MouseButton& MouseButton) const
    {
        return m_MouseButtonStates.Contains(MouseButton) && m_MouseButtonStates[MouseButton] == InputState::Pressed;
    }

    bool DesktopWindow::GetMouseButton(const MouseButton& MouseButton) const
    {
        const int State = glfwGetMouseButton(m_Handle, (int)MouseButton);
        return State == GLFW_PRESS;
    }

    bool DesktopWindow::GetMouseButtonUp(const MouseButton& MouseButton) const
    {
        return m_MouseButtonStates.Contains(MouseButton) && m_MouseButtonStates[MouseButton] == InputState::Released;
    }

    bool DesktopWindow::GetCombined(const Array<KeyCode>& KeyCodes, const Array<MouseButton>& MouseButtons) const
    {
        const bool AllKeyCodes = KeyCodes.All([this](const KeyCode& KeyCode) {
            return GetKey(KeyCode);
        });

        const bool AllMouseButtons = MouseButtons.All([this](const MouseButton& MouseButton) {
            return GetMouseButton(MouseButton);
        });
        return AllKeyCodes && AllMouseButtons;
    }

    bool DesktopWindow::IsGamepadConnected(const size_t ID)
    {
        return glfwJoystickPresent((int)ID);
    }

    BufferView<bool> DesktopWindow::GetGamepadButtons(const size_t ID)
    {
        return BufferView(m_GamepadStates[ID].Buttons, GAMEPAD_BUTTON_COUNT).As<bool>();
    }

    bool DesktopWindow::GetGamepadButtonDown(const size_t ID, const GamepadButton& Button)
    {
        return IsGamepadConnected(ID) && m_GamepadButtons[ID][(size_t)Button] == InputState::Pressed;
    }

    bool DesktopWindow::GetGamepadButtonUp(const size_t ID, const GamepadButton& Button)
    {
        return IsGamepadConnected(ID) && m_GamepadButtons[ID][(size_t)Button] == InputState::Released;
    }

    bool DesktopWindow::GetGamepadButton(const size_t ID, const GamepadButton& Button)
    {
        return IsGamepadConnected(ID) && m_GamepadStates[ID].Buttons[(size_t)Button];
    }

    Vector2 DesktopWindow::GetGamepadStick(const size_t ID, const GamepadThumbstick& Thumbstick)
    {
        Vector2 Axis;
        switch (Thumbstick)
        {
        case GamepadThumbstick::Left: Axis = {m_GamepadAxes[ID][0], -m_GamepadAxes[ID][1]}; break;
        case GamepadThumbstick::Right: Axis = {m_GamepadAxes[ID][2], -m_GamepadAxes[ID][3]}; break;
        }
        return Axis;
    }

    f32 DesktopWindow::GetGamepadLeftShoulder(const size_t ID)
    {
        return m_GamepadAxes[ID][4];
    }

    f32 DesktopWindow::GetGamepadRightShoulder(const size_t ID)
    {
        return m_GamepadAxes[ID][5];
    }
}
