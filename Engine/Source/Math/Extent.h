#pragma once
#include "Runtime/TypeTraits.h"


namespace Nova
{
    template<typename T> requires IsIntegerValue<T>
    struct Extent
    {
        T X, Y, Width, Height;
        constexpr explicit Extent(T X, T Y, T Width, T Height) : X(X), Y(Y), Width(Width), Height(Height) {}
    };
}
