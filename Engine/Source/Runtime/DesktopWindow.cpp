#include "DesktopWindow.h"
#include "Input/Input.h"
#include <GLFW/glfw3.h>

namespace Nova
{
    bool DesktopWindow::Initialize(const WindowCreateInfo& createInfo)
    {
        if (!glfwInit())
            return false;

        if (m_Handle)
        {
            glfwDestroyWindow(m_Handle);
            m_Handle = nullptr;
        }

        glfwWindowHint(GLFW_RESIZABLE, createInfo.resizable);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Handle = glfwCreateWindow(createInfo.width, createInfo.height, *createInfo.title, nullptr, nullptr);

        if (!m_Handle)
            return false;

        if (createInfo.show)
            glfwShowWindow(m_Handle);

        m_Width = createInfo.width;
        m_Height = createInfo.height;
        m_Title = createInfo.title;

        glfwSetWindowUserPointer(m_Handle, this);

        glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* handle)
        {
            DesktopWindow* window = (DesktopWindow*)glfwGetWindowUserPointer(handle);
            if (window->closeEvent.IsBound())
                window->closeEvent.Broadcast();
        });

        glfwSetWindowFocusCallback(m_Handle, [](GLFWwindow* handle, const int focus){
            DesktopWindow* window = (DesktopWindow*)glfwGetWindowUserPointer(handle);
            window->m_HasFocus = (bool)focus;
        });

        glfwSetWindowMaximizeCallback(m_Handle, [](GLFWwindow* handle, const int maximized){
            DesktopWindow* window = (DesktopWindow*)glfwGetWindowUserPointer(handle);
            window->m_IsMaximized = (bool)maximized;
            if (window->maximizeEvent.IsBound())
                window->maximizeEvent.Broadcast();
        });

        glfwSetWindowMaximizeCallback(m_Handle, [](GLFWwindow* handle, const int maximized){
            DesktopWindow* window = (DesktopWindow*)glfwGetWindowUserPointer(handle);
            window->m_IsMaximized = (bool)maximized;
            if (window->maximizeEvent.IsBound())
                window->maximizeEvent.Broadcast();
        });

        glfwSetWindowPosCallback(m_Handle, [](GLFWwindow* handle, const int newX, const int newY){
            DesktopWindow* window = (DesktopWindow*)glfwGetWindowUserPointer(handle);
            window->m_PosX = newX;
            window->m_PosY = newY;
        });

        glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow* handle, const int newWidth, const int newHeight){
            DesktopWindow* window = (DesktopWindow*)glfwGetWindowUserPointer(handle);
            window->m_Width = newWidth;
            window->m_Height = newHeight;
            if (window->resizeEvent.IsBound())
                window->resizeEvent.Broadcast(newWidth, newHeight);
        });

        glfwSetWindowIconifyCallback(m_Handle, [](GLFWwindow* handle, const int iconified)
        {
            DesktopWindow* window = (DesktopWindow*)glfwGetWindowUserPointer(handle);
            window->m_IsMinimized = (bool)iconified;
        });

        glfwSetKeyCallback(m_Handle, [](GLFWwindow* handle, int key, int, const int action, int)
        {
            DesktopWindow* window = (DesktopWindow*)glfwGetWindowUserPointer(handle);
            switch (action)
            {
            case GLFW_PRESS:
                window->m_KeyStates[(KeyCode)key] = InputState::Pressed;
                break;
            case GLFW_RELEASE:
                window->m_KeyStates[(KeyCode)key] = InputState::Released;
                break;
            default: break;
            }
        });

        glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* handle, int button, const int action, int)
        {
            DesktopWindow* window = (DesktopWindow*)glfwGetWindowUserPointer(handle);
            switch (action)
            {
            case GLFW_PRESS:
                window->m_MouseButtonStates[(MouseButton)button] = InputState::Pressed;
                break;
            case GLFW_RELEASE:
                window->m_MouseButtonStates[(MouseButton)button] = InputState::Released;
                break;
            default: break;
            }
        });

        return true;
    }

    void DesktopWindow::PollEvents()
    {
        for(auto& [key, state] : m_KeyStates)
        {
            state = InputState::None;
        }

        for(auto& [mouseButton, state] : m_MouseButtonStates)
        {
            state = InputState::None;
        }

        for(auto& gamepad : m_GamepadButtons)
        {
            for(size_t i = 0; i < GAMEPAD_BUTTON_COUNT; ++i)
                gamepad[i] = InputState::None;
        }

        m_LastGamepadStates = m_GamepadStates;

        for(size_t id = 0; id < GAMEPAD_COUNT; ++id)
        {
            glfwGetGamepadState((int)id, (GLFWgamepadstate*)&m_GamepadStates[id]);

            for(size_t buttonIndex = 0 ; buttonIndex < GAMEPAD_BUTTON_COUNT; ++buttonIndex)
            {
                if(m_GamepadStates[id].buttons[buttonIndex] != m_LastGamepadStates[id].buttons[buttonIndex])
                {
                    const InputState state = m_GamepadStates[id].buttons[buttonIndex] ? InputState::Pressed : InputState::Released;
                    m_GamepadButtons[id][buttonIndex] = state;
                }
            }

            for(size_t axis = 0 ; axis < GAMEPAD_AXIS_COUNT; ++axis)
            {
                m_GamepadAxes[id][axis] = m_GamepadStates[id].axes[axis];
            }

            m_LastGamepadStates[id] = m_GamepadStates[id];
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

    void DesktopWindow::Show() const
    {
        glfwShowWindow(m_Handle);
    }

    void DesktopWindow::Hide() const
    {
        glfwHideWindow(m_Handle);
    }

    void DesktopWindow::SetPosition(const int32_t x, const int32_t y)
    {
        glfwSetWindowPos(m_Handle, x, y);
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

    void DesktopWindow::SetCursorMode(const CursorMode& mode) const
    {
        switch (mode)
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
        double positionX = 0, positionY = 0;
        glfwGetCursorPos(m_Handle, &positionX, &positionY);
        return Vector2(positionX, positionY);
    }

    Vector2 DesktopWindow::GetDeltaMousePosition() const
    {
        return m_DeltaMousePosition;
    }

    void DesktopWindow::SetFullscreen(const bool enabled)
    {
        if (enabled)
        {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
            m_CachedPosX = m_PosX;
            m_CachedPosY = m_PosY;
            m_CachedWidth = m_Width;
            m_CachedHeight = m_Height;
            glfwSetWindowMonitor(m_Handle, monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
        } else
        {
            glfwSetWindowMonitor(m_Handle, nullptr, m_CachedPosX, m_CachedPosY, m_CachedWidth, m_CachedHeight, GLFW_DONT_CARE);
        }
    }

    void DesktopWindow::SetIcon(const void* pixels, const uint32_t width, const uint32_t height)
    {
        const GLFWimage image { (int32_t)width, (int32_t)height, (unsigned char*)pixels };
        glfwSetWindowIcon(m_Handle, 1, &image);
    }


    bool DesktopWindow::GetKeyDown(const KeyCode& keyCode) const
    {
        return m_KeyStates.Contains(keyCode) && m_KeyStates[keyCode] == InputState::Pressed;
    }

    bool DesktopWindow::GetKey(const KeyCode& keyCode) const
    {
        const int state = glfwGetKey(m_Handle, (int)keyCode);
        return state == GLFW_PRESS;
    }

    bool DesktopWindow::GetKeyUp(const KeyCode& keyCode) const
    {
        return m_KeyStates.Contains(keyCode) && m_KeyStates[keyCode] == InputState::Released;
    }

    bool DesktopWindow::GetMouseButtonDown(const MouseButton& mouseButton) const
    {
        return m_MouseButtonStates.Contains(mouseButton) && m_MouseButtonStates[mouseButton] == InputState::Pressed;
    }

    bool DesktopWindow::GetMouseButton(const MouseButton& mouseButton) const
    {
        const int state = glfwGetMouseButton(m_Handle, (int)mouseButton);
        return state == GLFW_PRESS;
    }

    bool DesktopWindow::GetMouseButtonUp(const MouseButton& mouseButton) const
    {
        return m_MouseButtonStates.Contains(mouseButton) && m_MouseButtonStates[mouseButton] == InputState::Released;
    }

    bool DesktopWindow::GetCombined(const Array<KeyCode>& keyCodes, const Array<MouseButton>& mouseButtons) const
    {
        const bool allKeyCodes = keyCodes.All([this](const KeyCode& keyCode) {
            return GetKey(keyCode);
        });

        const bool allMouseButtons = mouseButtons.All([this](const MouseButton& mouseButton) {
            return GetMouseButton(mouseButton);
        });
        return allKeyCodes && allMouseButtons;
    }

    bool DesktopWindow::IsGamepadConnected(const size_t id)
    {
        return glfwJoystickPresent((int)id);
    }

    BufferView<bool> DesktopWindow::GetGamepadButtons(const size_t id)
    {
        return BufferView(m_GamepadStates[id].buttons, GAMEPAD_BUTTON_COUNT).As<bool>();
    }

    bool DesktopWindow::GetGamepadButtonDown(const size_t id, const GamepadButton& Button)
    {
        return IsGamepadConnected(id) && m_GamepadButtons[id][(size_t)Button] == InputState::Pressed;
    }

    bool DesktopWindow::GetGamepadButtonUp(const size_t id, const GamepadButton& Button)
    {
        return IsGamepadConnected(id) && m_GamepadButtons[id][(size_t)Button] == InputState::Released;
    }

    bool DesktopWindow::GetGamepadButton(const size_t id, const GamepadButton& Button)
    {
        return IsGamepadConnected(id) && m_GamepadStates[id].buttons[(size_t)Button];
    }

    Vector2 DesktopWindow::GetGamepadStick(const size_t id, const GamepadThumbstick& Thumbstick)
    {
        Vector2 Axis;
        switch (Thumbstick)
        {
        case GamepadThumbstick::Left: Axis = {m_GamepadAxes[id][0], -m_GamepadAxes[id][1]}; break;
        case GamepadThumbstick::Right: Axis = {m_GamepadAxes[id][2], -m_GamepadAxes[id][3]}; break;
        }
        return Axis;
    }

    float DesktopWindow::GetGamepadLeftShoulder(const size_t id)
    {
        return m_GamepadAxes[id][4];
    }

    float DesktopWindow::GetGamepadRightShoulder(const size_t id)
    {
        return m_GamepadAxes[id][5];
    }
}
