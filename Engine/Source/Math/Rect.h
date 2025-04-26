#pragma once
#include "Runtime/TypeTraits.h"


namespace Nova
{
    template<typename T> requires IsArithmeticValue<T>
    struct Rect2D
    {
        T X, Y, Width, Height;

        constexpr Rect2D() : X((T)0), Y((T)0), Width((T)0), Height((T)0) {}
        constexpr explicit Rect2D(T X, T Y, T Width, T Height) : X(X), Y(Y), Width(Width), Height(Height) {}

        template<typename U> requires IsArithmeticValue<U>
        Rect2D<U> As()
        {
            return Rect2D<U>((U)X, (U)Y, (U)Width, (U)Height);
        }
    };

    template<typename T> requires IsArithmeticValue<T>
    struct Rect3D
    {
        T X, Y, Z, Width, Height, Depth;

        constexpr Rect3D() : X((T)0), Y((T)0), Z((T)0), Width((T)0), Height((T)0), Depth((T)0) {}
        constexpr explicit Rect3D(T X, T Y, T Width, T Height, T Depth) : X(X), Y(Y), Z(Width), Width(Height), Height(Depth) {}

        template<typename U> requires IsArithmeticValue<U>
        Rect3D<U> As()
        {
            return Rect3D<U>((U)X, (U)Y, (U)Z, (U)Width, (U)Height, (U)Depth);
        }
    };
}
