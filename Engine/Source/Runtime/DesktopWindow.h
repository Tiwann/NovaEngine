#pragma once
#include <functional>

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
    static constexpr uint32_t GAMEPAD_COUNT = 16;
    static constexpr uint32_t GAMEPAD_BUTTON_COUNT = 15;
    static constexpr uint32_t GAMEPAD_AXIS_COUNT = 6;

    struct GamepadState
    {
        uint8_t buttons[15];
        float axes[6];
    };

    class DesktopWindow : public Window
    {
    public:
        bool Initialize(const WindowCreateInfo& createInfo) override;
        void Update() override;
        void Destroy() override;

        GLFWwindow* GetHandle();
        const GLFWwindow* GetHandle() const;

        void Show() const;
        void Hide() const;
        void SetPosition(int32_t x, int32_t y);
        bool HasFocus() const;
        bool IsMinimized() const;
        bool IsMaximized() const;
        void SetCursorMode(const CursorMode& mode) const;
        Vector2 GetMousePosition() const;
        Vector2 GetDeltaMousePosition() const;

        bool GetKeyDown(const KeyCode& keyCode) const;
        bool GetKey(const KeyCode& keyCode) const;
        bool GetKeyUp(const KeyCode& keyCode) const;
        bool GetMouseButtonDown(const MouseButton& mouseButton) const;
        bool GetMouseButton(const MouseButton& mouseButton) const;
        bool GetMouseButtonUp(const MouseButton& mouseButton) const;
        bool GetCombined(const Array<KeyCode>& keyCodes, const Array<MouseButton>& mouseButtons) const;

        bool IsGamepadConnected(size_t id);
        BufferView<bool> GetGamepadButtons(size_t id);
        bool GetGamepadButtonDown(size_t id, const GamepadButton& button);
        bool GetGamepadButtonUp(size_t id, const GamepadButton& button);
        bool GetGamepadButton(size_t id, const GamepadButton& button);
        Vector2 GetGamepadStick(size_t id, const GamepadThumbstick& thumbstick);
        float GetGamepadLeftShoulder(size_t id);
        float GetGamepadRightShoulder(size_t id);

    private:
        GLFWwindow* m_Handle = nullptr;
        uint32_t m_PosX = 0, m_PosY = 0;
        bool m_HasFocus = false;
        bool m_IsMinimized = false;
        bool m_IsMaximized = false;

        Map<KeyCode, InputState> m_KeyStates;
        Map<MouseButton, InputState> m_MouseButtonStates;
        StaticArray<GamepadState, GAMEPAD_COUNT> m_GamepadStates;
        StaticArray<GamepadState, GAMEPAD_COUNT> m_LastGamepadStates;
        StaticArray<StaticArray<InputState, GAMEPAD_BUTTON_COUNT>, GAMEPAD_COUNT> m_GamepadButtons;
        StaticArray<StaticArray<float, GAMEPAD_AXIS_COUNT>, GAMEPAD_COUNT> m_GamepadAxes;

        Vector2 m_MousePosition;
        Vector2 m_LastMousePosition;
        Vector2 m_DeltaMousePosition;
    };
}
