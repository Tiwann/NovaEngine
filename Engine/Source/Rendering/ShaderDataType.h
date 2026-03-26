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
        case ShaderDataType::Mat4: return Format::None;
        case ShaderDataType::Int: return Format::R32_SINT;
        case ShaderDataType::Int2: return Format::R32G32_SINT;
        case ShaderDataType::Int3: return Format::R32G32B32_SINT;
        case ShaderDataType::Int4: return Format::R32G32B32A32_SINT;
        case ShaderDataType::UInt: return Format::R32_UINT;
        case ShaderDataType::UInt2: return Format::R32G32_UINT;
        case ShaderDataType::UInt3: return Format::R32G32B32_UINT;
        case ShaderDataType::UInt4: return Format::R32G32B32A32_UINT;
        default: return Format::None;
        }
    }
}
