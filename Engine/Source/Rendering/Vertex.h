#pragma once
#include "Math/LinearAlgebra.h"

namespace Nova
{
    struct Vertex
    {
        Vector3 Position{};
        Vector2 TextureCoordinate{};
        Vector3 Normal{};
        Vector4 Color{};

        enum class Attributes
        {
            Position,
            TextureCoordinate,
            Normal,
            Color,
        };
    };
}