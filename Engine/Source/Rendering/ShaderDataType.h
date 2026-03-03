#pragma once
#include "Runtime/Format.h"

namespace Nova
{
    enum class ShaderDataType
    {
        Float,
        Float2,
        Float3,
        Float4,
        Mat2,
        Mat3,
        Mat4,
        Int,
        Int2,
        Int3,
        Int4,
        UInt,
        UInt2,
        UInt3,
        UInt4,
    };

    inline uint32_t GetDataTypeSize(const ShaderDataType type)
    {
        switch(type)
        {
        case ShaderDataType::Float: return 1 * 4;
        case ShaderDataType::Float2: return 2 * 4;
        case ShaderDataType::Float3: return 3 * 4;
        case ShaderDataType::Float4: return 4 * 4;
        case ShaderDataType::Mat2: return 2 * 2 * 4;
        case ShaderDataType::Mat3: return 3 * 3 * 4;
        case ShaderDataType::Mat4: return 4 * 4 * 4;
        case ShaderDataType::Int: return  1 * 4;
        case ShaderDataType::Int2: return 2 * 4;
        case ShaderDataType::Int3: return 3 * 4;
        case ShaderDataType::Int4: return 4 * 4;
        case ShaderDataType::UInt: return 1 * 4;
        case ShaderDataType::UInt2: return 2 * 4;
        case ShaderDataType::UInt3: return 3 * 4;
        case ShaderDataType::UInt4: return 4 * 4;
        default: return 0;
        }
    }

    inline Format GetFormat(const ShaderDataType type)
    {
        switch(type)
        {
        case ShaderDataType::Float: return Format::Float;
        case ShaderDataType::Float2: return Format::Float2;
        case ShaderDataType::Float3: return Format::Float3;
        case ShaderDataType::Float4: return Format::Float4;
        case ShaderDataType::Mat2:
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
        case ShaderDataType::UInt:
        case ShaderDataType::UInt2:
        case ShaderDataType::UInt3:
        case ShaderDataType::UInt4: return Format::None;
        default: return Format::None;
        }
    }
}
