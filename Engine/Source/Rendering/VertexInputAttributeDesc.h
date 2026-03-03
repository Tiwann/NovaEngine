#pragma once
#include "Runtime/Format.h"
#include <cstdint>

namespace Nova
{
    struct VertexInputAttributeDesc
    {
        uint32_t location = 0;
        uint32_t binding = 0;
        Format format = Format::None;
        uint32_t offset = 0;
    };
}
