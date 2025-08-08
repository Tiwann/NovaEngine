#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Nova
{
    struct SpriteVertex
    {
        Vector3 Position;
        Vector2 TextureCoordinate;
        Vector3 Normal;
        Vector4 Color;

        enum class Attributes
        {
            Position,
            TextureCoordinate,
            Normal,
            Color,
        };
    };
}