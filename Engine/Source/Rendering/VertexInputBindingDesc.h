#pragma once
#include "VertexInputRate.h"
#include <cstdint>

namespace Nova
{
    struct VertexInputBindingDesc
    {
        uint32_t binding = 0;
        uint32_t stride = 0;
        VertexInputRate inputRate = VertexInputRate::Vertex;
    };
}