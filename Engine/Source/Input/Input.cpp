#include "Input.h"
#include "Runtime/Application.h"
#include "Runtime/Window.h"
#include "Math/Vector2.h"

#include <GLFW/glfw3.h>

namespace Nova
{
    bool Input::GetKeyDown(const KeyCode KeyCode)
    {
        return s_KeyStates[KeyCode] == InputState::Pressed;
    }

    bool Input::GetKey(KeyCode KeyCode)
    {
        Window* Window = g_Application->GetWindow();
        const int State = glfwGetKey(Window->GetNativeWindow(), (int)KeyCode);
        return State == GLFW_PRESS;
    }

    bool Input::GetKeyUp(const KeyCode KeyCode)
    {
        return s_KeyStates[KeyCode] == InputState::Released;
    }

    String Input::GetKeyName(const KeyCode KeyCode)
    {
        switch (KeyCode) {
        case KeyCode::Unknown: return "Unknown Key";
        case KeyCode::Space: return "Space";
        case KeyCode::Apostrophe: return "Apostrophe";
        case KeyCode::Comma: return "Comma";
        case KeyCode::Minus: return "Minus";
        case KeyCode::Period: return "Period";
        case KeyCode::Slash: return "Slash";
        case KeyCode::Key0: return "Key 0";
        case KeyCode::Key1: return "Key 1";
        case KeyCode::Key2: return "Key 2";
        case KeyCode::Key3: return "Key 3";
        case KeyCode::Key4: return "Key 4";
        case KeyCode::Key5: return "Key 5";
        case KeyCode::Key6: return "Key 6";
        case KeyCode::Key7: return "Key 7";
        case KeyCode::Key8: return "Key 8";
        case KeyCode::Key9: return "Key 9";
        case KeyCode::SemiColon: return "Semicolon";
        case KeyCode::Equal: return "Equal";
        case KeyCode::KeyA: return "A Key";
        case KeyCode::KeyB: return "B Key";
        case KeyCode::KeyC: return "C Key";
        case KeyCode::KeyD: return "D Key";
        case KeyCode::KeyE: return "E Key";
        case KeyCode::KeyF: return "F Key";
        case KeyCode::KeyG: return "G Key";
        case KeyCode::KeyH: return "H Key";
        case KeyCode::KeyI: return "I Key";
        case KeyCode::KeyJ: return "J Key";
        case KeyCode::KeyK: return "K Key";
        case KeyCode::KeyL: return "L Key";
        case KeyCode::KeyM: return "M Key";
        case KeyCode::KeyN: return "N Key";
        case KeyCode::KeyO: return "O Key";
        case KeyCode::KeyP: return "P Key";
        case KeyCode::KeyQ: return "Q Key";
        case KeyCode::KeyR: return "R Key";
        case KeyCode::KeyS: return "S Key";
        case KeyCode::KeyT: return "T Key";
        case KeyCode::KeyU: return "U Key";
        case KeyCode::KeyV: return "V Key";
        case KeyCode::KeyW: return "W Key";
        case KeyCode::KeyX: return "X Key";
        case KeyCode::KeyY: return "Y Key";
        case KeyCode::KeyZ: return "Z Key";
        case KeyCode::LeftBracket: return "Left Bracket";
        case KeyCode::BackSlash: return "Backslash";
        case KeyCode::RightBracket: return "Right Bracket";
        case KeyCode::GraveAccent: return "Grave Accent";
        case KeyCode::World1: return "World 1";
        case KeyCode::World2: return "World 2";
        case KeyCode::Escape: return "Escape";
        case KeyCode::Enter: return "Enter";
        case KeyCode::Tab: return "Tab";
        case KeyCode::Backspace: return "Backspace";
        case KeyCode::Insert: return "Insert";
        case KeyCode::Delete: return "Delete";
        case KeyCode::Right: return "Right";
        case KeyCode::Left: return "Left";
        case KeyCode::Down: return "Down";
        case KeyCode::Up: return "Up";
        case KeyCode::PageUp: return "Page Up";
        case KeyCode::PageDown: return "Page Down";
        case KeyCode::Home: return "Home";
        case KeyCode::End: return "End";
        case KeyCode::CapsLock: return "Caps Lock";
        case KeyCode::ScrollLock: return "Scroll Lock";
        case KeyCode::NumLock: return "Num Lock";
        case KeyCode::PrintScreen: return "Print Screen";
        case KeyCode::Pause: return "Pause";
        case KeyCode::F1:  return "F1";
        case KeyCode::F2:  return "F2";
        case KeyCode::F3:  return "F3";
        case KeyCode::F4:  return "F4";
        case KeyCode::F5:  return "F5";
        case KeyCode::F6:  return "F6";
        case KeyCode::F7:  return "F7";
        case KeyCode::F8:  return "F8";
        case KeyCode::F9:  return "F9";
        case KeyCode::F10: return "F10";
        case KeyCode::F11: return "F11";
        case KeyCode::F12: return "F12";
        case KeyCode::F13: return "F13";
        case KeyCode::F14: return "F14";
        case KeyCode::F15: return "F15";
        case KeyCode::F16: return "F16";
        case KeyCode::F17: return "F17";
        case KeyCode::F18: return "F18";
        case KeyCode::F19: return "F19";
        case KeyCode::F20: return "F20";
        case KeyCode::F21: return "F21";
        case KeyCode::F22: return "F22";
        case KeyCode::F23: return "F23";
        case KeyCode::F24: return "F24";
        case KeyCode::F25: return "F25";
        case KeyCode::Numpad0: return "Numpad 0";
        case KeyCode::Numpad1: return "Numpad 1";
        case KeyCode::Numpad2: return "Numpad 2";
        case KeyCode::Numpad3: return "Numpad 3";
        case KeyCode::Numpad4: return "Numpad 4";
        case KeyCode::Numpad5: return "Numpad 5";
        case KeyCode::Numpad6: return "Numpad 6";
        case KeyCode::Numpad7: return "Numpad 7";
        case KeyCode::Numpad8: return "Numpad 8";
        case KeyCode::Numpad9: return "Numpad 9";
        case KeyCode::NumpadDecimal: return "Numpad Decimal";
        case KeyCode::NumpadDivide: return "Numpad Divide";
        case KeyCode::NumpadMultiply: return "Numpad Multiply";
        case KeyCode::NumpadSubtract: return "Numpad Subtract";
        case KeyCode::NumpadAdd: return "Numpad Add";
        case KeyCode::NumpadEnter: return "Numpad Enter";
        case KeyCode::NumpadEqual: return "Numpad Equal";
        case KeyCode::LeftShift: return "Left Shift";
        case KeyCode::LeftControl: return "Left Control";
        case KeyCode::LeftAlt: return "Left Alt";
        case KeyCode::LeftSuper: return "Left Super";
        case KeyCode::RightShift: return "Left Shift";
        case KeyCode::RightControl: return "Right Control";
        case KeyCode::RightAlt: return "Right Alt";
        case KeyCode::RightSuper: return "Right Super";
        case KeyCode::Menu: return "Menu";
        }
        return "Unknown";
    }

    f32 Input::GetAxis(KeyCode Negative, KeyCode Positive)
    {
        f32 Result = 0.0f;
        if(GetKey(Negative)) Result -= 1.0f;
        if(GetKey(Positive)) Result += 1.0f;
        return Result;
    }

    Vector2 Input::Get2DAxis(KeyCode Up, KeyCode Down, KeyCode Left, KeyCode Right)
    {
        return { GetAxis(Left, Right), GetAxis(Up, Down)};
    }

    bool Input::GetMouseButtonDown(const MouseButton MouseButton)
    {
        return s_MouseButtonStates[MouseButton] == InputState::Pressed;
    }

    bool Input::GetMouseButton(MouseButton MouseButton)
    {
        Window* Window = g_Application->GetWindow();
        const int State = glfwGetMouseButton(Window->GetNativeWindow(), (int)MouseButton);
        return State == GLFW_PRESS;
    }

    bool Input::GetMouseButtonUp(const MouseButton MouseButton)
    {
        return s_MouseButtonStates[MouseButton] == InputState::Released;
    }

    Vector2 Input::GetMousePosition()
    {
        GLFWwindow* NativeWindow = g_Application->GetWindow()->GetNativeWindow();
        f64 X, Y;
        glfwGetCursorPos(NativeWindow, &X, &Y);
        return {(f32)X, (f32)Y};
    }

    Vector2 Input::GetMouseDelta()
    {
        return s_DeltaMousePosition;
    }

    bool Input::GetCombined(const Array<KeyCode>& KeyCodes, const Array<MouseButton>& MouseButtons)
    {
        const bool AllKeyCodes = KeyCodes.All([](const KeyCode& KeyCode) {
            return Input::GetKey(KeyCode);
        });

        const bool AllMouseButtons = MouseButtons.All([](const MouseButton& MouseButton) {
            return Input::GetMouseButton(MouseButton);
        });
        return AllKeyCodes && AllMouseButtons;
    }

    bool Input::IsGamepadConnected(const size_t ID)
    {
        return glfwJoystickPresent((int)ID);
    }

    BufferView<bool> Input::GetGamepadButtons(const size_t ID)
    {
        return BufferView(s_GamepadStates[ID].buttons, 15).As<bool>();
    }

    bool Input::GetGamepadButtonDown(const size_t ID, GamepadButton Button)
    {
        return IsGamepadConnected(ID) && s_GamepadButtons[ID][(size_t)Button] == InputState::Pressed;
    }

    bool Input::GetGamepadButtonUp(const size_t ID, GamepadButton Button)
    {
        return IsGamepadConnected(ID) && s_GamepadButtons[ID][(size_t)Button] == InputState::Released;
    }

    bool Input::GetGamepadButton(size_t ID, GamepadButton Button)
    {
        return IsGamepadConnected(ID) && s_GamepadStates[ID].buttons[(size_t)Button];
    }

    Vector2 Input::GetGamepadStick(size_t ID, GamepadThumbstick Thumbstick, f32 Deadzone)
    {
        Vector2 Axis;
        switch (Thumbstick)
        {
        case GamepadThumbstick::Left: Axis = {s_GamepadAxes[ID][0], -s_GamepadAxes[ID][1]}; break;
        case GamepadThumbstick::Right: Axis = {s_GamepadAxes[ID][2], -s_GamepadAxes[ID][3]}; break;
        }
        return Axis.Magnitude() < Deadzone ? Vector2::Zero : Axis;
    }

    f32 Input::GetGamepadLeftShoulder(size_t ID)
    {
        return s_GamepadAxes[ID][4];
    }

    f32 Input::GetGamepadRightShoulder(size_t ID)
    {
        return s_GamepadAxes[ID][5];
    }
}
