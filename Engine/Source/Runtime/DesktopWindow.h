#pragma once
#include "Window.h"
#include "Containers/Map.h"
#include "Containers/StaticArray.h"
#include "Input/InputState.h"
#include "Input/KeyCode.h"
#include "Input/MouseButton.h"
#include "Math/Vector2.h"

struct GLFWwindow;

namespace Nova
{
    struct GamepadState
    {
        u8 Buttons[15];
        f32 Axes[6];
    };

    class DesktopWindow : public Window
    {
    public:
        explicit DesktopWindow(Application* Owner);
        bool Initialize(const WindowCreateInfo& CreateInfo) override;
        void Update(f32 Delta) override;
        void Destroy() override;

        GLFWwindow* GetHandle();
        const GLFWwindow* GetHandle() const;

        void Show();
        void Hide();
        void SetPosition(i32 NewPosX, i32 NewPosY);
        bool HasFocus() const;
        bool IsMinimized() const;
        bool IsMaximized() const;
        void SetCursorHidden();
        Vector2 GetMousePosition() const;
        Vector2 GetDeltaMousePosition() const;

        bool GetKeyDown(KeyCode KeyCode) const;
        bool GetKey(KeyCode KeyCode) const;
        bool GetKeyUp(KeyCode KeyCode) const;
        bool GetMouseButtonDown(MouseButton MouseButton) const;
        bool GetMouseButton(MouseButton MouseButton) const;
        bool GetMouseButtonUp(MouseButton MouseButton) const;
    private:
        GLFWwindow* m_Handle = nullptr;
        u32 m_PosX = 0, m_PosY = 0;
        bool m_HasFocus = false;
        bool m_IsMinimized = false;
        bool m_IsMaximized = false;

        Map<KeyCode, InputState> m_KeyStates;
        Map<MouseButton, InputState> m_MouseButtonStates;
        StaticArray<GamepadState, 16> m_GamepadStates;
        StaticArray<GamepadState, 16> m_LastGamepadStates;
        StaticArray<StaticArray<InputState, 15>, 16> m_GamepadButtons;
        StaticArray<StaticArray<f32, 6>, 16> m_GamepadAxes;

        Vector2 m_MousePosition;
        Vector2 m_LastMousePosition;
        Vector2 m_DeltaMousePosition;
    };
}
