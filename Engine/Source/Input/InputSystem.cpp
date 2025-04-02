#include "InputSystem.h"

namespace Nova
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
