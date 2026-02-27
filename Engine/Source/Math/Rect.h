#pragma once
#include "Runtime/TypeTraits.h"

namespace Nova
{
    template<typename T> requires IsArithmeticValue<T>
    struct Rect2D
    {
        T x, y, width, height;

        constexpr Rect2D() : x((T)0), y((T)0), width((T)0), height((T)0) {}
        constexpr Rect2D(T x, T y, T width, T height) : x(x), y(y), width(width), height(height) {}

        template<typename U> requires IsArithmeticValue<U>
        Rect2D<U> As()
        {
            return Rect2D<U>((U)x, (U)y, (U)width, (U)height);
        }
    };

    template<typename T> requires IsArithmeticValue<T>
    struct Rect3D
    {
        T x, y, z, width, height, depth;

        constexpr Rect3D() : x((T)0), y((T)0), z((T)0), width((T)0), height((T)0), depth((T)0) {}
        constexpr explicit Rect3D(T x, T y, T z, T width, T height, T depth) : x(x), y(y), z(z), width(width), height(height), depth(depth) {}

        template<typename U> requires IsArithmeticValue<U>
        Rect3D<U> As()
        {
            return Rect3D<U>((U)x, (U)y, (U)z, (U)width, (U)height, (U)depth);
        }
    };
}
