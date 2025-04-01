#pragma once
#include "Core/NumericTypes.h"
#include "Core/Containers/String.h"
#include "Macros.h"

namespace Hydro
{
    enum class GamepadThumbstick { Left, Right };

    inline String GamepadThumbstickToString(GamepadThumbstick Stick)
    {
        switch (Stick)
        {
            case GamepadThumbstick::Left: return "Left Thumbstick";
            case GamepadThumbstick::Right: return "Right Thumbstick";
        }
        return "Unknown";
    }
}

HYDRO_ENUM_FMT(Hydro::GamepadThumbstick, Hydro::GamepadThumbstickToString);
