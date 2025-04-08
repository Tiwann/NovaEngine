#pragma once
#include "Containers/MulticastDelegate.h"
#include "Containers/String.h"
#include "Types.h"

namespace Nova
{
    struct AnimationNotify
    {
        String Name;
        u32 Frame;
        MulticastDelegate<void()> Event;
    };
}