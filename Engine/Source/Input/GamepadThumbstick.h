#pragma once
#include "Containers/String.h"


namespace Nova
{
    enum class GamepadThumbstick { Left, Right };

    inline String GamepadThumbstickToString(GamepadThumbstick stick)
    {
        switch (stick)
        {
            case GamepadThumbstick::Left: return "Left Thumbstick";
            case GamepadThumbstick::Right: return "Right Thumbstick";
        }
        return "Unknown";
    }
}
