#pragma once
#include <cstdint>

namespace Nova::OpenGL
{
    struct GLformat
    {
        uint32_t internalFormat;
        uint32_t format;
        uint32_t type;
        bool compressed;
    };
}