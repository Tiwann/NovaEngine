#pragma once
#include "Runtime/Types.h"
#include "Containers/String.h"
#include <CoreExport.h>

namespace Nova
{
    enum class InputState
    {
        None,
        Pressed,
        Released,
    };

    inline String CORE_API InputStateToString(InputState State)
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
