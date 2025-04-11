#pragma once
#include "Containers/String.h"
#include <CoreExport.h>

namespace Nova
{
    enum class GamepadThumbstick { Left, Right };

    inline String CORE_API GamepadThumbstickToString(GamepadThumbstick Stick)
    {
        switch (Stick)
        {
            case GamepadThumbstick::Left: return "Left Thumbstick";
            case GamepadThumbstick::Right: return "Right Thumbstick";
        }
        return "Unknown";
    }
}
