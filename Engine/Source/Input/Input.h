#pragma once
#include "KeyCode.h"
#include "Containers/String.h"
#include "Runtime/LogCategory.h"
#include <CoreExport.h>


NOVA_DECLARE_LOG_CATEGORY_STATIC(Input, "INPUT")

struct GLFWgamepadstate;

namespace Nova
{
    constexpr i32 NOVA_MAX_GAMEPADS = 16;

    struct Vector2;
    
    class CORE_API Input
    {
    public:
        friend class Application;
        static String GetKeyName(KeyCode KeyCode);
    };
}
