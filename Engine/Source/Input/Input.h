#pragma once
#include "Containers/StaticArray.h"
#include "GamepadButton.h"
#include "GamepadThumbstick.h"
#include "KeyCode.h"
#include "InputState.h"
#include "MouseButton.h"
#include "Containers/Buffer.h"
#include "Containers/String.h"
#include "Containers/Map.h"
#include "Runtime/LogCategory.h"
#include <CoreExport.h>


NOVA_DECLARE_LOG_CATEGORY_STATIC(Input, "INPUT")

struct GLFWgamepadstate;

namespace Nova
{
    constexpr i32 NOVA_MAX_GAMEPADS = 16;

    struct Vector2;
    
    class CORE_API Input
    {
    public:
        friend class Application;
        static bool GetKeyDown(KeyCode KeyCode);
        static bool GetKey(KeyCode KeyCode);
        static bool GetKeyUp(KeyCode KeyCode);
        static String GetKeyName(KeyCode KeyCode);
        static f32 GetAxis(KeyCode Negative, KeyCode Positive);
        static Vector2 Get2DAxis(KeyCode Up, KeyCode Down, KeyCode Left, KeyCode Right);

        static bool GetMouseButtonDown(MouseButton MouseButton);
        static bool GetMouseButton(MouseButton MouseButton);
        static bool GetMouseButtonUp(MouseButton MouseButton);
        
        static Vector2 GetMousePosition();
        static Vector2 GetMouseDelta();

        static bool GetCombined(const Array<KeyCode>& KeyCodes, const Array<MouseButton>& MouseButtons);

        static bool IsGamepadConnected(size_t ID);
        static BufferView<bool> GetGamepadButtons(size_t ID);
        static bool GetGamepadButtonDown(size_t ID, GamepadButton Button);
        static bool GetGamepadButtonUp(size_t ID, GamepadButton Button);
        static bool GetGamepadButton(size_t ID, GamepadButton Button);
        static Vector2 GetGamepadStick(size_t ID, GamepadThumbstick Thumbstick, f32 Deadzone = 0);
        static f32 GetGamepadLeftShoulder(size_t ID);
        static f32 GetGamepadRightShoulder(size_t ID);        
    private:
        inline static Map<KeyCode, InputState> s_KeyStates;
        inline static Map<MouseButton, InputState> s_MouseButtonStates;

        static StaticArray<GLFWgamepadstate, NOVA_MAX_GAMEPADS> s_GamepadStates;
        static StaticArray<GLFWgamepadstate, NOVA_MAX_GAMEPADS> s_LastGamepadStates;
        inline static StaticArray<StaticArray<InputState, 15>, NOVA_MAX_GAMEPADS> s_GamepadButtons;
        inline static StaticArray<StaticArray<f32, 6>, NOVA_MAX_GAMEPADS> s_GamepadAxes;
        static Vector2 s_DeltaMousePosition;
        
        static void UpdateGamepads();
        static void ResetInputStates();
    };
}
