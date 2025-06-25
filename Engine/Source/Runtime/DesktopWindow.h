#pragma once
#include "Window.h"
#include "Containers/Map.h"
#include "Containers/StaticArray.h"
#include "Input/CursorMode.h"
#include "Input/InputState.h"
#include "Input/KeyCode.h"
#include "Input/MouseButton.h"
#include "Input/GamepadButton.h"
#include "Input/GamepadThumbstick.h"
#include "Math/Vector2.h"

struct GLFWwindow;


namespace Nova
{
    static constexpr u32 GAMEPAD_COUNT = 16;
    static constexpr u32 GAMEPAD_BUTTON_COUNT = 15;
    static constexpr u32 GAMEPAD_AXIS_COUNT = 6;

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

        void Show() const;
        void Hide() const;
        void SetPosition(i32 NewPosX, i32 NewPosY);
        bool HasFocus() const;
        bool IsMinimized() const;
        bool IsMaximized() const;
        void SetCursorMode(const CursorMode& Mode) const;
        Vector2 GetMousePosition() const;
        Vector2 GetDeltaMousePosition() const;

        bool GetKeyDown(const KeyCode& KeyCode) const;
        bool GetKey(const KeyCode& KeyCode) const;
        bool GetKeyUp(const KeyCode& KeyCode) const;
        bool GetMouseButtonDown(const MouseButton& MouseButton) const;
        bool GetMouseButton(const MouseButton& MouseButton) const;
        bool GetMouseButtonUp(const MouseButton& MouseButton) const;
        bool GetCombined(const Array<KeyCode>& KeyCodes, const Array<MouseButton>& MouseButtons) const;

        bool IsGamepadConnected(size_t ID);
        BufferView<bool> GetGamepadButtons(size_t ID);
        bool GetGamepadButtonDown(size_t ID, const GamepadButton& Button);
        bool GetGamepadButtonUp(size_t ID, const GamepadButton& Button);
        bool GetGamepadButton(size_t ID, const GamepadButton& Button);
        Vector2 GetGamepadStick(size_t ID, const GamepadThumbstick& Thumbstick);
        f32 GetGamepadLeftShoulder(size_t ID);
        f32 GetGamepadRightShoulder(size_t ID);
    private:
        GLFWwindow* m_Handle = nullptr;
        u32 m_PosX = 0, m_PosY = 0;
        bool m_HasFocus = false;
        bool m_IsMinimized = false;
        bool m_IsMaximized = false;

        Map<KeyCode, InputState> m_KeyStates;
        Map<MouseButton, InputState> m_MouseButtonStates;
        StaticArray<GamepadState, GAMEPAD_COUNT> m_GamepadStates;
        StaticArray<GamepadState, GAMEPAD_COUNT> m_LastGamepadStates;
        StaticArray<StaticArray<InputState, GAMEPAD_BUTTON_COUNT>, GAMEPAD_COUNT> m_GamepadButtons;
        StaticArray<StaticArray<f32, GAMEPAD_AXIS_COUNT>, GAMEPAD_COUNT> m_GamepadAxes;

        Vector2 m_MousePosition;
        Vector2 m_LastMousePosition;
        Vector2 m_DeltaMousePosition;
    };
}
