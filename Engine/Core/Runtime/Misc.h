#pragma once
#include "TypeTraits.h"

namespace Nova
{
    template<typename T> requires IsFloatingPointValue<T>
    constexpr T FPS(int Val) { return 1.0f / (T)Val; }
}
