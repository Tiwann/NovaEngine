#pragma once
#include "Runtime/Types.h"
#include "Containers/String.h"
#include <InputExport.h>

namespace Nova
{
    enum class InputState
    {
        None,
        Pressed,
        Released,
    };

    static String INPUT_API InputStateToString(InputState State)
    {
        switch (State)
        {
        case InputState::None: return "None";
        case InputState::Pressed: return "Pressed";
        case InputState::Released: return "Released";
        }
        return "None";
    }
}
