#pragma once
#include <cassert>

namespace Nova
{
    constexpr void Assert(bool Condition, const char* Message)
    {
        assert(Condition && Message);
    }
}