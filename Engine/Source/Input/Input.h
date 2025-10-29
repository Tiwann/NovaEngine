#pragma once
#include "KeyCode.h"
#include "Containers/String.h"


namespace Nova
{
    class Input
    {
    public:
        friend class Application;
        static String GetKeyName(KeyCode KeyCode);
    };
}
