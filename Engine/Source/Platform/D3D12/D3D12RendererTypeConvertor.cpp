#pragma once
#include "D3D12RendererTypeConvertor.h"

namespace Nova
{
    D3D12_CULL_MODE D3D12RendererTypeConvertor::ConvertCullMode(const CullMode Mode) const
    {
        switch (Mode)
        {
        case CullMode::FrontFace: return D3D12_CULL_MODE_FRONT;
        case CullMode::BackFace: return D3D12_CULL_MODE_BACK;
        case CullMode::FrontAndBackFaces: throw;
        case CullMode::None: return D3D12_CULL_MODE_NONE;
        default: throw;
        }
    }

    D3D12_BLEND D3D12RendererTypeConvertor::ConvertBlendFactor(const BlendFactor Factor) const
    {
        switch (Factor)
        {
        case BlendFactor::Zero: return D3D12_BLEND_ZERO;
        case BlendFactor::One: return D3D12_BLEND_ONE;
        case BlendFactor::SourceColor: return D3D12_BLEND_SRC_COLOR;
        case BlendFactor::OneMinusSourceColor: return D3D12_BLEND_INV_SRC_COLOR;
        case BlendFactor::DestColor: return D3D12_BLEND_DEST_COLOR;
        case BlendFactor::OneMinusDestColor: return D3D12_BLEND_INV_DEST_COLOR;
        case BlendFactor::SourceAlpha: return D3D12_BLEND_SRC_ALPHA;
        case BlendFactor::OneMinusSourceAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
        case BlendFactor::DestAlpha: return D3D12_BLEND_DEST_ALPHA;
        case BlendFactor::OneMinusDestAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
        case BlendFactor::ConstantColor: return D3D12_BLEND_BLEND_FACTOR;
        case BlendFactor::OnMinusConstantColor: return D3D12_BLEND_INV_BLEND_FACTOR;
        case BlendFactor::ConstantAlpha: throw;
        case BlendFactor::OneMinusConstantAlpha: throw;
        case BlendFactor::SourceAlphaSaturated: return D3D12_BLEND_SRC_ALPHA_SAT;
        case BlendFactor::Source1Color: return D3D12_BLEND_SRC1_COLOR;
        case BlendFactor::OneMinusSource1Color: return D3D12_BLEND_INV_SRC1_COLOR;
        case BlendFactor::Source1Alpha: return D3D12_BLEND_SRC1_ALPHA;
        case BlendFactor::OneMinusSource1Alpha: return D3D12_BLEND_INV_SRC1_ALPHA;
        default: throw;
        }
    }

    D3D12_BLEND_OP D3D12RendererTypeConvertor::ConvertBlendOperation(const BlendOperation Operation) const
    {
        switch (Operation)
        {
        case BlendOperation::Add: return D3D12_BLEND_OP_ADD;
        case BlendOperation::Min: return D3D12_BLEND_OP_MIN;
        case BlendOperation::Max: return D3D12_BLEND_OP_MAX;
        case BlendOperation::Subtract: return D3D12_BLEND_OP_SUBTRACT;
        case BlendOperation::ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
        default: throw;
        }
    }

    D3D12_FILTER_TYPE D3D12RendererTypeConvertor::ConvertFilter(const Filter Filter) const
    {
        switch (Filter)
        {
        case Filter::Nearest: return D3D12_FILTER_TYPE_POINT;
        case Filter::Linear: return D3D12_FILTER_TYPE_LINEAR;
        default: throw;
        }
    }

    D3D12_FRONT_FACE D3D12RendererTypeConvertor::ConvertFrontFace(const FrontFace Face) const
    {
        switch (Face)
        {
        case FrontFace::Clockwise: return D3D12_FRONT_FACE_CLOCKWISE;
        case FrontFace::CounterClockwise: return D3D12_FRONT_FACE_COUNTER_CLOCKWISE;
        default: throw;
        }
    }

    D3D12_FILL_MODE D3D12RendererTypeConvertor::ConvertPolygonMode(const PolygonMode Mode) const
    {
        switch (Mode)
        {
        case PolygonMode::Fill: return D3D12_FILL_MODE_SOLID;
        case PolygonMode::Line: return D3D12_FILL_MODE_WIREFRAME;
        case PolygonMode::Point:
        default: throw;
        }
    }

    D3D12_PRIMITIVE_TOPOLOGY_TYPE D3D12RendererTypeConvertor::ConvertPrimitiveTopology(const PrimitiveTopology Topology) const
    {
        switch (Topology)
        {
        case PrimitiveTopology::PointList:     return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        case PrimitiveTopology::LineList:
        case PrimitiveTopology::LineStrip:     return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case PrimitiveTopology::TriangleList:
        case PrimitiveTopology::TriangleStrip:
        case PrimitiveTopology::TriangleFan:   return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        default: throw;
        }
    }

    D3D12_TEXTURE_ADDRESS_MODE D3D12RendererTypeConvertor::ConvertSamplerAddressMode(const SamplerAddressMode Address) const
    {
        switch (Address)
        {
        case SamplerAddressMode::Repeat:              return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case SamplerAddressMode::MirroredRepeat:      return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case SamplerAddressMode::ClampToEdge:         return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case SamplerAddressMode::ClampToBorder:       return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        case SamplerAddressMode::MirrorClampToEdge:   return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
        default: throw;
        }
    }

    D3D12_SHADER_VISIBILITY D3D12RendererTypeConvertor::ConvertShaderStage(const ShaderStageFlagBits Stage) const
    {
        switch (Stage)
        {
        case ShaderStageFlagBits::None:              throw;
        case ShaderStageFlagBits::Vertex:            return D3D12_SHADER_VISIBILITY_VERTEX;
        case ShaderStageFlagBits::Geometry:          return D3D12_SHADER_VISIBILITY_GEOMETRY;
        case ShaderStageFlagBits::Fragment:          return D3D12_SHADER_VISIBILITY_PIXEL;
        case ShaderStageFlagBits::Compute:           throw;
        case ShaderStageFlagBits::RayGeneration:     throw;
        case ShaderStageFlagBits::Tessellation:      throw;
        case ShaderStageFlagBits::Mesh:              return D3D12_SHADER_VISIBILITY_MESH;
        default: throw;
        }
    }

    D3D12_COMPARISON_FUNC D3D12RendererTypeConvertor::ConvertCompareOperation(const CompareOperation Operation) const
    {
        switch (Operation)
        {
        case CompareOperation::Always:           return D3D12_COMPARISON_FUNC_ALWAYS;
        case CompareOperation::Never:            return D3D12_COMPARISON_FUNC_NEVER;
        case CompareOperation::Less:             return D3D12_COMPARISON_FUNC_LESS;
        case CompareOperation::LessOrEqual:      return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case CompareOperation::Equal:            return D3D12_COMPARISON_FUNC_EQUAL;
        case CompareOperation::NotEqual:         return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case CompareOperation::Greater:          return D3D12_COMPARISON_FUNC_GREATER;
        case CompareOperation::GreaterOrEqual:   return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        default: throw;
        }
    }

    DXGI_FORMAT D3D12RendererTypeConvertor::ConvertFormat(const Format Format) const
    {
        switch (Format)
        {
        case Format::None:                         return DXGI_FORMAT_UNKNOWN;
        case Format::R8_UNORM:                     return DXGI_FORMAT_R8_UNORM;
        case Format::R8_SNORM:                     return DXGI_FORMAT_R8_SNORM;
        case Format::R16_USHORT:                   return DXGI_FORMAT_R16_UINT;
        case Format::R16_SHORT:                    return DXGI_FORMAT_R16_SINT;
        case Format::R32_FLOAT:                    return DXGI_FORMAT_R32_FLOAT;
        case Format::R32_UINT:                     return DXGI_FORMAT_R32_UINT;
        case Format::R32_SINT:                     return DXGI_FORMAT_R32_SINT;
        case Format::R8G8_UNORM:                   return DXGI_FORMAT_R8G8_UNORM;
        case Format::R8G8_SNORM:                   return DXGI_FORMAT_R8G8_SNORM;
        case Format::R16G16_USHORT:                return DXGI_FORMAT_R16G16_UINT;
        case Format::R16G16_SHORT:                 return DXGI_FORMAT_R16G16_SINT;
        case Format::R32G32_UINT:                  return DXGI_FORMAT_R32G32_UINT;
        case Format::R32G32_SINT:                  return DXGI_FORMAT_R32G32_SINT;
        case Format::R32G32_FLOAT:                 return DXGI_FORMAT_R32G32_FLOAT;
        case Format::R8G8B8_UNORM:                 return DXGI_FORMAT_UNKNOWN; // DXGI n'a pas de format RGB 24 bits
        case Format::R8G8B8_SNORM:                 return DXGI_FORMAT_UNKNOWN;
        case Format::R16G16B16_USHORT:             return DXGI_FORMAT_UNKNOWN;
        case Format::R16G16B16_SHORT:              return DXGI_FORMAT_UNKNOWN;
        case Format::R32G32B32_UINT:               return DXGI_FORMAT_R32G32B32_UINT;
        case Format::R32G32B32_SINT:               return DXGI_FORMAT_R32G32B32_SINT;
        case Format::R32G32B32_FLOAT:              return DXGI_FORMAT_R32G32B32_FLOAT;
        case Format::R8G8B8A8_UNORM:               return DXGI_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8_SNORM:               return DXGI_FORMAT_R8G8B8A8_SNORM;
        case Format::R8G8B8A8_SRGB:                return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case Format::R16G16B16A16_USHORT:          return DXGI_FORMAT_R16G16B16A16_UINT;
        case Format::R16G16B16A16_SHORT:           return DXGI_FORMAT_R16G16B16A16_SINT;
        case Format::R32G32B32A32_UINT:            return DXGI_FORMAT_R32G32B32A32_UINT;
        case Format::R32G32B32A32_SINT:            return DXGI_FORMAT_R32G32B32A32_SINT;
        case Format::R32G32B32A32_FLOAT:           return DXGI_FORMAT_R32G32B32A32_FLOAT;
        default: throw;
        }
    }

    DXGI_SWAP_EFFECT D3D12RendererTypeConvertor::ConvertPresentMode(const PresentMode PresentMode) const
    {
        switch (PresentMode)
        {
        case PresentMode::Unknown: throw;
        case PresentMode::Immediate: return DXGI_SWAP_EFFECT_DISCARD;
        case PresentMode::Mailbox: return DXGI_SWAP_EFFECT_FLIP_DISCARD;
        case PresentMode::Fifo: return DXGI_SWAP_EFFECT_FLIP_DISCARD;
        default: throw;
        }
    }
}
