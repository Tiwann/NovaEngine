#pragma once
#include "Containers/String.h"
#include <CoreExport.h>

namespace Nova
{
    enum class GamepadButton
    {
        Unknown = -1,
        ButtonSouth = 0,
        ButtonEast,
        ButtonWest,
        ButtonNorth,
        LeftShoulder,
        RightShoulder,
        SpecialLeft,
        SpecialRight,
        Home,
        LeftThumbstick,
        RightThumbstick,
        DPadUp,
        DPadRight,
        DPadDown,
        DPadLeft,
        MaxButtons
    };

    inline String CORE_API GamepadButtonToString(GamepadButton Button)
    {
        switch (Button)
        {
        default: return "Unknown";
        case GamepadButton::Unknown: return "Unknown";
        case GamepadButton::ButtonSouth: return "ButtonSouth";
        case GamepadButton::ButtonEast: return "ButtonEast";
        case GamepadButton::ButtonWest: return "ButtonWest";
        case GamepadButton::ButtonNorth: return "ButtonNorth";
        case GamepadButton::LeftShoulder: return "LeftShoulder";
        case GamepadButton::RightShoulder: return "RightShoulder";
        case GamepadButton::SpecialLeft: return "SpecialLeft";
        case GamepadButton::SpecialRight: return "SpecialRight";
        case GamepadButton::Home: return "Home";
        case GamepadButton::LeftThumbstick: return "LeftThumbstick";
        case GamepadButton::RightThumbstick: return "RightThumbstick";
        case GamepadButton::DPadUp: return "DPadUp";
        case GamepadButton::DPadRight: return "DPadRight";
        case GamepadButton::DPadDown: return "DPadDown";
        case GamepadButton::DPadLeft: return "DPadLeft";

        }
        return "Unknown";
    }
}