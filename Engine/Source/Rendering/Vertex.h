#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Nova
{
    struct Vertex
    {
        Vector3 position;
        Vector2 texCoords;
        Vector3 normal;
        Vector4 color;

        enum class Attributes
        {
            Position,
            TextureCoordinates,
            Normal,
            Color,
        };
    };
}