#include "OpenGLRendererTypeConvertor.h"
#include <glad/gl.h>

namespace Nova
{
    GLenum OpenGLRendererTypeConvertor::ConvertCullMode(const CullMode Mode) const
    {
        switch (Mode)
        {
        case CullMode::FrontFace: return GL_FRONT;
        case CullMode::BackFace: return GL_BACK;
        case CullMode::FrontAndBackFaces: return GL_FRONT_AND_BACK;
        case CullMode::None: return GL_NONE;
        default: throw;
        }
    }

    GLenum OpenGLRendererTypeConvertor::ConvertBlendFactor(const BlendFactor Factor) const
    {
        switch (Factor)
        {
        case BlendFactor::Zero: return GL_ZERO;
        case BlendFactor::One: return GL_ONE;
        case BlendFactor::SourceColor: return GL_SRC_COLOR;
        case BlendFactor::OneMinusSourceColor: return GL_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DestColor: return GL_DST_COLOR;
        case BlendFactor::OneMinusDestColor: return GL_ONE_MINUS_DST_COLOR;
        case BlendFactor::SourceAlpha: return GL_SRC_ALPHA;
        case BlendFactor::OneMinusSourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DestAlpha: return GL_DST_ALPHA;
        case BlendFactor::OneMinusDestAlpha: return GL_ONE_MINUS_DST_ALPHA;
        case BlendFactor::ConstantColor: return GL_CONSTANT_COLOR;
        case BlendFactor::OnMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::ConstantAlpha: return GL_CONSTANT_ALPHA;
        case BlendFactor::OneMinusConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SourceAlphaSaturated: return GL_SRC_ALPHA_SATURATE;
        case BlendFactor::Source1Color: return GL_SRC1_COLOR;
        case BlendFactor::OneMinusSource1Color: return GL_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::Source1Alpha: return GL_SRC1_ALPHA;
        case BlendFactor::OneMinusSource1Alpha: return GL_ONE_MINUS_SRC1_ALPHA;
        default: throw;
        }
    }

        GLenum OpenGLRendererTypeConvertor::ConvertBlendOperation(const BlendOperation Operation) const
    {
        switch (Operation)
        {
        case BlendOperation::Add: return GL_FUNC_ADD;
        case BlendOperation::Subtract: return GL_FUNC_SUBTRACT;
        case BlendOperation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
        case BlendOperation::Min: return GL_MIN;
        case BlendOperation::Max: return GL_MAX;
        default: throw;
        }
    }

    GLenum OpenGLRendererTypeConvertor::ConvertFilter(const Filter Filter) const
    {
        switch (Filter)
        {
        case Filter::Nearest: return GL_NEAREST;
        case Filter::Linear: return GL_LINEAR;
        default: throw;
        }
    }

    GLenum OpenGLRendererTypeConvertor::ConvertFrontFace(const FrontFace Face) const
    {
        switch (Face)
        {
        case FrontFace::Clockwise: return GL_CW;
        case FrontFace::CounterClockwise: return GL_CCW;
        default: throw;
        }
    }

    GLenum OpenGLRendererTypeConvertor::ConvertPolygonMode(const PolygonMode Mode) const
    {
        switch (Mode)
        {
        case PolygonMode::Fill: return GL_FILL;
        case PolygonMode::Line: return GL_LINE;
        case PolygonMode::Point: return GL_POINT;
        default: throw;
        }
    }

    GLenum OpenGLRendererTypeConvertor::ConvertPrimitiveTopology(const PrimitiveTopology Topology) const
    {
        switch (Topology)
        {
        case PrimitiveTopology::PointList: return GL_POINTS;
        case PrimitiveTopology::LineList: return GL_LINES;
        case PrimitiveTopology::LineStrip: return GL_LINE_STRIP;
        case PrimitiveTopology::TriangleList: return GL_TRIANGLES;
        case PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
        case PrimitiveTopology::TriangleFan: return GL_TRIANGLE_FAN;
        default: throw;
        }
    }

    GLenum OpenGLRendererTypeConvertor::ConvertSamplerAddressMode(const SamplerAddressMode Address) const
    {
        switch (Address)
        {
        case SamplerAddressMode::Repeat: return GL_REPEAT;
        case SamplerAddressMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
        case SamplerAddressMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
        case SamplerAddressMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
        default: throw;
        }
    }

    GLenum OpenGLRendererTypeConvertor::ConvertShaderStage(const ShaderStage Stage) const
    {
        switch (Stage)
        {
        case ShaderStage::Vertex: return GL_VERTEX_SHADER;
        case ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
        case ShaderStage::Geometry: return GL_GEOMETRY_SHADER;
        case ShaderStage::Compute: return GL_COMPUTE_SHADER;
        default: throw;
        }
    }

    GLenum OpenGLRendererTypeConvertor::ConvertCompareOperation(const CompareOperation Operation) const
    {
        switch (Operation)
        {
        case CompareOperation::Never: return GL_NEVER;
        case CompareOperation::Less: return GL_LESS;
        case CompareOperation::Equal: return GL_EQUAL;
        case CompareOperation::LessOrEqual: return GL_LEQUAL;
        case CompareOperation::Greater: return GL_GREATER;
        case CompareOperation::NotEqual: return GL_NOTEQUAL;
        case CompareOperation::GreaterOrEqual: return GL_GEQUAL;
        case CompareOperation::Always: return GL_ALWAYS;
        default: throw;
        }
    }

    GLenum OpenGLRendererTypeConvertor::ConvertFormat(const Format Format) const
    {
        switch (Format)
        {
        case Format::NONE: return GL_NONE;
        case Format::R8_UNORM: return GL_R8;
        case Format::R8_SNORM: return GL_R8_SNORM;
        case Format::R16_USHORT: return GL_R16UI;
        case Format::R16_SHORT: return GL_R16I;
        case Format::R32_FLOAT: return GL_R32F;
        case Format::R32_UINT: return GL_R32UI;
        case Format::R32_SINT: return GL_R32I;
        case Format::R8G8_UNORM: return GL_RG8;
        case Format::R8G8_SNORM: return GL_RG8_SNORM;
        case Format::R16G16_USHORT: return GL_RG16UI;
        case Format::R16G16_SHORT: return GL_RG16I;
        case Format::R32G32_UINT: return GL_RG32UI;
        case Format::R32G32_SINT: return GL_RG32I;
        case Format::R32G32_FLOAT: return GL_RG32F;
        case Format::R8G8B8_UNORM: return GL_RGB8;
        case Format::R8G8B8_SNORM: return GL_RGB8_SNORM;
        case Format::R16G16B16_USHORT: return GL_RGB16UI;
        case Format::R16G16B16_SHORT: return GL_RGB16I;
        case Format::R32G32B32_UINT: return GL_RGB32UI;
        case Format::R32G32B32_SINT: return GL_RGB32I;
        case Format::R32G32B32_FLOAT: return GL_RGB32F;
        case Format::R8G8B8A8_UNORM: return GL_RGBA8;
        case Format::R8G8B8A8_SNORM: return GL_RGBA8_SNORM;
        case Format::R16G16B16A16_USHORT: return GL_RGBA16UI;
        case Format::R16G16B16A16_SHORT: return GL_RGBA16I;
        case Format::R32G32B32A32_UINT: return GL_RGBA32UI;
        case Format::R32G32B32A32_SINT: return GL_RGBA32I;
        case Format::R32G32B32A32_FLOAT: return GL_RGBA32F;
        default: throw;
        }
    }

    GLenum OpenGLRendererTypeConvertor::ConvertPresentMode(const PresentMode PresentMode) const
    {
        throw;
    }
}
