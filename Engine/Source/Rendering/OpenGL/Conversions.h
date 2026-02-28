#pragma once
#include "Runtime/Format.h"
#include "Rendering/ShaderStage.h"
#include "Format.h"
#include <glad/glad.h>

#include "Rendering/CompareOperation.h"
#include "Rendering/Filter.h"
#include "Rendering/SamplerAddressMode.h"


namespace Nova::OpenGL
{
    template<typename Out, typename In>
    Out Convert(const In& value);

    template<>
    inline GLFormat Convert(const Format& format)
    {
        switch (format)
        {
        case Format::None: return {0, 0, 0, false};
        case Format::R8_UNORM: return {GL_R8, GL_RED, GL_UNSIGNED_BYTE, false};
        case Format::R8_SNORM: return {GL_R8_SNORM, GL_RED, GL_BYTE, false};
        case Format::R16_USHORT: return {GL_R16, GL_RED, GL_UNSIGNED_SHORT, false};
        case Format::R16_SHORT: return {GL_R16_SNORM, GL_RED, GL_SHORT, false};
        case Format::R32_FLOAT: return {GL_R32F, GL_RED, GL_FLOAT, false};
        case Format::R32_UINT: return {GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, false};
        case Format::R32_SINT: return {GL_R32I, GL_RED_INTEGER, GL_INT, false};
        case Format::R8G8_UNORM: return {GL_RG8, GL_RG, GL_UNSIGNED_BYTE, false};
        case Format::R8G8_SNORM: return {GL_RG8_SNORM, GL_RG, GL_BYTE, false};
        case Format::R16G16_USHORT: return {GL_RG16, GL_RG, GL_UNSIGNED_SHORT, false};
        case Format::R16G16_SHORT: return {GL_RG16_SNORM, GL_RG, GL_SHORT, false};
        case Format::R32G32_FLOAT: return {GL_RG32F, GL_RG, GL_FLOAT, false};
        case Format::R32G32_UINT: return {GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT, false};
        case Format::R32G32_SINT: return {GL_RG32I, GL_RG_INTEGER, GL_INT, false};
        case Format::R8G8B8_UNORM: return {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, false};
        case Format::R8G8B8_SNORM: return {GL_RGB8_SNORM, GL_RGB, GL_BYTE, false};
        case Format::R16G16B16_USHORT: return {GL_RGB16, GL_RGB, GL_UNSIGNED_SHORT, false};
        case Format::R16G16B16_SHORT: return {GL_RGB16_SNORM, GL_RGB, GL_SHORT, false};
        case Format::R32G32B32_FLOAT: return {GL_RGB32F, GL_RGB, GL_FLOAT, false};
        case Format::R32G32B32_UINT: return {GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT, false};
        case Format::R32G32B32_SINT: return {GL_RGB32I, GL_RGB_INTEGER, GL_INT, false};
        case Format::R8G8B8A8_UNORM: return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, false};
        case Format::R8G8B8A8_SNORM: return {GL_RGBA8_SNORM, GL_RGBA, GL_BYTE, false};
        case Format::R8G8B8A8_SRGB: return {GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, false};
        case Format::R16G16B16A16_USHORT: return {GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT, false};
        case Format::R16G16B16A16_SHORT: return {GL_RGBA16_SNORM, GL_RGBA, GL_SHORT, false};
        case Format::R32G32B32A32_FLOAT: return {GL_RGBA32F, GL_RGBA, GL_FLOAT, false};
        case Format::R32G32B32A32_UINT: return {GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT, false};
        case Format::R32G32B32A32_SINT: return {GL_RGBA32I, GL_RGBA_INTEGER, GL_INT, false};
        case Format::R64G64B64A64_FLOAT: return {GL_RGBA, GL_RGBA, GL_DOUBLE, false};
        case Format::D32_FLOAT: return {GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, false};
        case Format::D24_UNORM_S8_UINT: return {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, false};
        case Format::D32_FLOAT_S8_UINT: return {GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, false};

        default:
            return { 0, 0, 0, false };
        }
    }

    template<>
    inline GLenum Convert(const ShaderStageFlagBits& stage)
    {
        switch (stage)
        {
        case ShaderStageFlagBits::None: return GL_INVALID_ENUM;
        case ShaderStageFlagBits::Vertex: return GL_VERTEX_SHADER;
        case ShaderStageFlagBits::Geometry: return GL_GEOMETRY_SHADER;
        case ShaderStageFlagBits::Fragment: return GL_FRAGMENT_SHADER;
        case ShaderStageFlagBits::Compute: return GL_COMPUTE_SHADER;
        case ShaderStageFlagBits::RayGeneration: return GL_INVALID_ENUM; // NOT SUPPORTED
        case ShaderStageFlagBits::TessellationControl: return GL_TESS_CONTROL_SHADER;
        case ShaderStageFlagBits::TessellationEvaluation: return GL_TESS_EVALUATION_SHADER;
        case ShaderStageFlagBits::Mesh: return GL_INVALID_ENUM; // NOT SUPPORTED
        default: return GL_INVALID_ENUM;
        }
    }

    template<>
    inline GLenum Convert(const Filter& filter)
    {
        switch (filter)
        {
        case Filter::Nearest: return GL_NEAREST;
        case Filter::Linear: return GL_LINEAR;
        default: return GL_LINEAR;
        }
    }

    template<>
    inline GLenum Convert(const SamplerAddressMode& mode)
    {
        switch (mode)
        {
        case SamplerAddressMode::Repeat: return GL_REPEAT;
        case SamplerAddressMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
        case SamplerAddressMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
        case SamplerAddressMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
        case SamplerAddressMode::MirrorClampToEdge: return GL_MIRROR_CLAMP_TO_EDGE;
        default: return GL_REPEAT;
        }
    }

    template<>
    inline GLenum Convert(const CompareOperation& op)
    {
        switch (op)
        {
        case CompareOperation::Always: return GL_ALWAYS;
        case CompareOperation::Never: return GL_NEVER;
        case CompareOperation::Less: return GL_LESS;
        case CompareOperation::LessOrEqual: return GL_LEQUAL;
        case CompareOperation::Equal: return GL_EQUAL;
        case CompareOperation::NotEqual: return GL_NOTEQUAL;
        case CompareOperation::Greater: return GL_GREATER;
        case CompareOperation::GreaterOrEqual: return GL_GEQUAL;
        default: return GL_GREATER;
        }
    }
}
