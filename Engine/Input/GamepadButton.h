#pragma once
#include "Core/NumericTypes.h"
#include "Core/Containers/String.h"
#include "Core/Containers/StringView.h"
#include "Core/Containers/StringFormat.h"

namespace Hydro
{
    enum class GamepadButton
    {
        Unknown = U8_MAX,
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

    inline String GamepadButtonToString(GamepadButton Button)
    {
        switch (Button)
        {
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

#include <format>
HYDRO_ENUM_FMT(Hydro::GamepadButton, Hydro::GamepadButtonToString);
