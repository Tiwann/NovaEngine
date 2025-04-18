#pragma once

namespace Nova
{
    enum class ShaderStage
    {
        None,
        Vertex,
        Geometry,
        Fragment,
        Compute,
        RayGeneration,
        Tessellation,
        Mesh
    };
}