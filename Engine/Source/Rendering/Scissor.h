#pragma once
#include "Runtime/Types.h"

namespace Nova
{
    struct Scissor
    {
        struct Offset
        {
            i32 X = 0, Y = 0;
        } Offset;

        struct Extent
        {
            i32 Width = 0, Height = 0;
        } Extent;
    };
}