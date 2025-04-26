#pragma once
#include "VulkanRendererTypeConvertor.h"

namespace Nova
{
    VkCullModeFlags VulkanRendererTypeConvertor::ConvertCullMode(const CullMode Mode) const
    {
        switch (Mode)
        {
        case CullMode::FrontFace:           return VK_CULL_MODE_FRONT_BIT;
        case CullMode::BackFace:            return VK_CULL_MODE_BACK_BIT;
        case CullMode::FrontAndBackFaces:   return VK_CULL_MODE_FRONT_BIT | VK_CULL_MODE_BACK_BIT;
        case CullMode::None:                return VK_CULL_MODE_NONE;
        }
        return VK_CULL_MODE_NONE;
    }

    VkBlendFactor VulkanRendererTypeConvertor::ConvertBlendFactor(const BlendFactor Factor) const
    {
        switch (Factor) {
        case BlendFactor::Zero: return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::One: return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SourceColor: return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::OneMinusSourceColor: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DestColor: return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::OneMinusDestColor: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::SourceAlpha: return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::OneMinusSourceAlpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DestAlpha: return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::OneMinusDestAlpha: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case BlendFactor::ConstantColor: return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case BlendFactor::OnMinusConstantColor: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::ConstantAlpha: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case BlendFactor::OneMinusConstantAlpha: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SourceAlphaSaturated: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case BlendFactor::Source1Color: return VK_BLEND_FACTOR_SRC1_COLOR;
        case BlendFactor::OneMinusSource1Color: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::Source1Alpha: return VK_BLEND_FACTOR_SRC1_ALPHA;
        case BlendFactor::OneMinusSource1Alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        }
        throw;
    }

    VkBlendOp VulkanRendererTypeConvertor::ConvertBlendOperation(const BlendOperation Operation) const
    {
        switch (Operation)
        {
        case BlendOperation::Add: return VK_BLEND_OP_ADD;
        case BlendOperation::Min: return VK_BLEND_OP_MIN;
        case BlendOperation::Max: return VK_BLEND_OP_MAX;
        case BlendOperation::Subtract: return VK_BLEND_OP_SUBTRACT;
        case BlendOperation::ReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
        }
        throw;
    }

    VkFilter VulkanRendererTypeConvertor::ConvertFilter(const Filter Filter) const
    {
        switch (Filter) {
        case Filter::Nearest: return VK_FILTER_NEAREST;
        case Filter::Linear: return VK_FILTER_LINEAR;
        }
        throw;
    }

    VkFrontFace VulkanRendererTypeConvertor::ConvertFrontFace(const FrontFace Face) const
    {
        switch (Face)
        {
        case FrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
        case FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        default: throw;
        }
    }

    VkPolygonMode VulkanRendererTypeConvertor::ConvertPolygonMode(const PolygonMode Mode) const
    {
        switch (Mode)
        {
        case PolygonMode::Fill:; return VK_POLYGON_MODE_FILL;
        case PolygonMode::Line: return VK_POLYGON_MODE_LINE;
        case PolygonMode::Point: return VK_POLYGON_MODE_POINT;
        default: throw;
        }
    }

    VkPrimitiveTopology VulkanRendererTypeConvertor::ConvertPrimitiveTopology(const PrimitiveTopology Topology) const
    {
        switch (Topology)
        {
        case PrimitiveTopology::PointList: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case PrimitiveTopology::LineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case PrimitiveTopology::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case PrimitiveTopology::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case PrimitiveTopology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case PrimitiveTopology::TriangleFan: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        default: throw;
        }
    }

    VkSamplerAddressMode VulkanRendererTypeConvertor::ConvertSamplerAddressMode(const SamplerAddressMode Address) const
    {
        switch (Address)
        {
        case SamplerAddressMode::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case SamplerAddressMode::MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case SamplerAddressMode::ClampToEdge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case SamplerAddressMode::ClampToBorder: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case SamplerAddressMode::MirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        }
        throw;
    }

    VkShaderStageFlagBits VulkanRendererTypeConvertor::ConvertShaderStage(const ShaderStage Stage) const
    {
        switch (Stage) {
        case ShaderStage::None: throw;
        case ShaderStage::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
        case ShaderStage::RayGeneration: return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        case ShaderStage::Tessellation: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case ShaderStage::Mesh: return VK_SHADER_STAGE_MESH_BIT_EXT;
        default: throw;
        }

    }

    VkCompareOp VulkanRendererTypeConvertor::ConvertCompareOperation(const CompareOperation Operation) const
    {
        switch (Operation)
        {
        case CompareOperation::Always:         return VK_COMPARE_OP_ALWAYS;
        case CompareOperation::Never:          return VK_COMPARE_OP_NEVER;
        case CompareOperation::Less:           return VK_COMPARE_OP_LESS;
        case CompareOperation::LessOrEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOperation::Equal:          return VK_COMPARE_OP_EQUAL;
        case CompareOperation::NotEqual:       return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOperation::Greater:        return VK_COMPARE_OP_GREATER;
        case CompareOperation::GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
        }
        throw;
    }

    VkFormat VulkanRendererTypeConvertor::ConvertFormat(const Format Format) const
    {
        switch (Format)
        {
        case Format::NONE:                  return VK_FORMAT_UNDEFINED;
        case Format::R8_UNORM:              return VK_FORMAT_R8_UNORM;
        case Format::R8_SNORM:              return VK_FORMAT_R8_SNORM;
        case Format::R16_USHORT:            return VK_FORMAT_R16_UINT;
        case Format::R16_SHORT:             return VK_FORMAT_R16_SINT;
        case Format::R32_FLOAT:             return VK_FORMAT_R32_SFLOAT;
        case Format::R32_UINT:              return VK_FORMAT_R32_UINT;
        case Format::R32_SINT:              return VK_FORMAT_R32_SINT;
        case Format::R8G8_UNORM:            return VK_FORMAT_R8G8_UNORM;
        case Format::R8G8_SNORM:            return VK_FORMAT_R8G8_SNORM;
        case Format::R16G16_USHORT:         return VK_FORMAT_R16G16_UINT;
        case Format::R16G16_SHORT:          return VK_FORMAT_R16G16_SINT;
        case Format::R32G32_UINT:           return VK_FORMAT_R32G32_UINT;
        case Format::R32G32_SINT:           return VK_FORMAT_R32G32_SINT;
        case Format::R32G32_FLOAT:          return VK_FORMAT_R32G32_SFLOAT;
        case Format::R8G8B8_UNORM:          return VK_FORMAT_R8G8B8_UNORM;
        case Format::R8G8B8_SNORM:          return VK_FORMAT_R8G8B8_SNORM;
        case Format::R16G16B16_USHORT:      return VK_FORMAT_R16G16B16_UINT;
        case Format::R16G16B16_SHORT:       return VK_FORMAT_R16G16B16_SINT;
        case Format::R32G32B32_UINT:        return VK_FORMAT_R32G32B32_UINT;
        case Format::R32G32B32_SINT:        return VK_FORMAT_R32G32B32_SINT;
        case Format::R32G32B32_FLOAT:       return VK_FORMAT_R32G32B32_SFLOAT;
        case Format::R8G8B8A8_UNORM:        return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8_SNORM:        return VK_FORMAT_R8G8B8A8_SNORM;
        case Format::R8G8B8A8_SRGB:         return VK_FORMAT_R8G8B8A8_SRGB;
        case Format::R16G16B16A16_USHORT:   return VK_FORMAT_R16G16B16A16_UINT;
        case Format::R16G16B16A16_SHORT:    return VK_FORMAT_R16G16B16A16_SINT;
        case Format::R32G32B32A32_UINT:     return VK_FORMAT_R32G32B32A32_UINT;
        case Format::R32G32B32A32_SINT:     return VK_FORMAT_R32G32B32A32_SINT;
        case Format::R32G32B32A32_FLOAT:    return VK_FORMAT_R32G32B32A32_SFLOAT;
        default: return VK_FORMAT_UNDEFINED;
        }
    }

    VkPresentModeKHR VulkanRendererTypeConvertor::ConvertPresentMode(const PresentMode PresentMode) const
    {
        switch (PresentMode)
        {
        case PresentMode::Unknown: throw;
        case PresentMode::Immediate: return VK_PRESENT_MODE_IMMEDIATE_KHR;
        case PresentMode::Mailbox: return VK_PRESENT_MODE_MAILBOX_KHR;
        case PresentMode::Fifo: return VK_PRESENT_MODE_FIFO_KHR;
        default: throw;
        }
    }
}
