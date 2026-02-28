#pragma once
#include "Runtime/Flags.h"

namespace Nova
{
    enum class ShaderStageFlagBits
    {
        None = 0,
        Vertex = BIT(0),
        Geometry = BIT(1),
        Fragment = BIT(2),
        Compute = BIT(3),
        RayGeneration = BIT(4),
        TessellationControl = BIT(5),
        TessellationEvaluation = BIT(6),
        Mesh = BIT(7),
    };

    typedef Flags<ShaderStageFlagBits> ShaderStageFlags;
}
