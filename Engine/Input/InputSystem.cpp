#include "InputSystem.h"

namespace Hydro
{
    String InputAction::GetAssetType() const
    {
        return "InputAction";
    }

    String InputActionMap::GetAssetType() const
    {
        return "InputActionMap";
    }
}
