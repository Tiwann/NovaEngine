#pragma once
#include "Containers/String.h"
#include "Containers/Function.h"

namespace Nova
{
    struct MenuItem
    {
        String Name;
        bool* Selected = nullptr;
        Function<void()> Callback = nullptr;
        bool Enabled = true;
    };
}
