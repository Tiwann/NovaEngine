#pragma once
#include "Containers/String.h"


namespace Nova
{
    enum class InputState
    {
        None,
        Pressed,
        Released,
    };

    inline String InputStateToString(InputState state)
    {
        switch (state)
        {
        case InputState::None: return "None";
        case InputState::Pressed: return "Pressed";
        case InputState::Released: return "Released";
        }
        return "None";
    }
}
