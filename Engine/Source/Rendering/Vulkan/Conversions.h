#pragma once
#include "Rendering/BlendFactor.h"
#include "Rendering/BlendOperation.h"
#include "Rendering/CompareOperation.h"
#include "Rendering/CullMode.h"
#include "Rendering/DescriptorType.h"
#include "Rendering/FrontFace.h"
#include "Rendering/PolygonMode.h"
#include "Rendering/PrimitiveTopology.h"
#include "Rendering/SamplerAddressMode.h"
#include "Rendering/PresentMode.h"
#include "Rendering/Filter.h"
#include "Runtime/Format.h"
#include "Rendering/ShaderStage.h"

#include <vulkan/vulkan.h>

#include "Rendering/LoadOperation.h"
#include "Rendering/StoreOperation.h"


namespace Nova::Vulkan
{
    template<typename NovaType, typename VulkanType>
    VulkanType Convert(const NovaType& value);

    template<>
    inline VkFormat Convert<Format, VkFormat>(const Format& value)
    {
        switch (value)
        {
        case Format::None:                  return VK_FORMAT_UNDEFINED;
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
        case Format::D32_FLOAT: return VK_FORMAT_D32_SFLOAT;
        case Format::D32_FLOAT_S8_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case Format::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
        default: return VK_FORMAT_UNDEFINED;
        }
    }

    template<>
    inline VkPresentModeKHR Convert<PresentMode, VkPresentModeKHR>(const PresentMode& value)
    {
        switch (value)
        {
        case PresentMode::Unknown: throw;
        case PresentMode::Immediate: return VK_PRESENT_MODE_IMMEDIATE_KHR;
        case PresentMode::Mailbox: return VK_PRESENT_MODE_MAILBOX_KHR;
        case PresentMode::Fifo: return VK_PRESENT_MODE_FIFO_KHR;
        default: throw;
        }
    }

    template<>
    inline VkFilter Convert<Filter, VkFilter>(const Filter& filter)
    {
        switch (filter)
        {
        case Filter::Nearest: return VK_FILTER_NEAREST;
        case Filter::Linear: return VK_FILTER_LINEAR;
        default: return VK_FILTER_NEAREST;
        }
    }

    template<>
    inline VkPrimitiveTopology Convert<PrimitiveTopology, VkPrimitiveTopology>(const PrimitiveTopology& value)
    {
        switch (value)
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

    template<>
    inline VkCullModeFlags Convert<CullMode, VkCullModeFlags>(const CullMode& value)
    {
        switch (value)
        {
        case CullMode::FrontFace:           return VK_CULL_MODE_FRONT_BIT;
        case CullMode::BackFace:            return VK_CULL_MODE_BACK_BIT;
        case CullMode::FrontAndBackFaces:   return VK_CULL_MODE_FRONT_BIT | VK_CULL_MODE_BACK_BIT;
        case CullMode::None:                return VK_CULL_MODE_NONE;
        default: return VK_CULL_MODE_NONE;
        }
    }

    template<>
    inline VkFrontFace Convert<FrontFace, VkFrontFace>(const FrontFace& value)
    {
        switch (value)
        {
        case FrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
        case FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        default: throw;
        }
    }

    template<>
    inline VkPolygonMode Convert<PolygonMode, VkPolygonMode>(const PolygonMode& value)
    {
        switch (value)
        {
        case PolygonMode::Fill:; return VK_POLYGON_MODE_FILL;
        case PolygonMode::Line: return VK_POLYGON_MODE_LINE;
        case PolygonMode::Point: return VK_POLYGON_MODE_POINT;
        default: throw;
        }
    }

    template<>
    inline VkCompareOp Convert<CompareOperation, VkCompareOp>(const CompareOperation& value)
    {
        switch (value)
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

    template<>
    inline VkSamplerAddressMode Convert<SamplerAddressMode, VkSamplerAddressMode>(const SamplerAddressMode& value)
    {
        switch (value)
        {
        case SamplerAddressMode::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case SamplerAddressMode::MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case SamplerAddressMode::ClampToEdge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case SamplerAddressMode::ClampToBorder: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case SamplerAddressMode::MirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        }
        throw;
    }

    template<>
    inline VkBlendOp Convert<BlendOperation, VkBlendOp>(const BlendOperation& value)
    {
        switch (value)
        {
        case BlendOperation::Add: return VK_BLEND_OP_ADD;
        case BlendOperation::Min: return VK_BLEND_OP_MIN;
        case BlendOperation::Max: return VK_BLEND_OP_MAX;
        case BlendOperation::Subtract: return VK_BLEND_OP_SUBTRACT;
        case BlendOperation::ReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
        }
        throw;
    }

    template<>
    inline VkBlendFactor Convert<BlendFactor, VkBlendFactor>(const BlendFactor& value)
    {
        switch (value) {
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

    template<>
    inline VkIndexType Convert<Format, VkIndexType>(const Format& value)
    {
        switch (value)
        {
        case Format::R32_UINT: return VK_INDEX_TYPE_UINT32;
        case Format::R16_USHORT : return VK_INDEX_TYPE_UINT16;
        case Format::R8_UNORM : return VK_INDEX_TYPE_UINT8;
        default: throw;
        }
    }

    template<>
    inline VkDescriptorType Convert<DescriptorType, VkDescriptorType>(const DescriptorType& value)
    {
        switch (value)
        {
        case DescriptorType::Sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
        case DescriptorType::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case DescriptorType::SampledImage: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case DescriptorType::StorageImage: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case DescriptorType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case DescriptorType::StorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case DescriptorType::InputAttachment: return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        case DescriptorType::InlineUniformBlock: return VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK;
        case DescriptorType::AccelerationStructure: return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        case DescriptorType::StorageTexelBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        case DescriptorType::UniformTexelBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        default: throw;
        }
    }

    template<>
    inline VkSamplerMipmapMode Convert<Filter, VkSamplerMipmapMode>(const Filter& value)
    {
        switch (value)
        {
        case Filter::Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case Filter::Linear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        default: throw;
        }
    }

    template<>
    inline VkAttachmentLoadOp Convert<Rendering::LoadOperation, VkAttachmentLoadOp>(const Rendering::LoadOperation& value)
    {
        switch (value)
        {
        case Rendering::LoadOperation::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
        case Rendering::LoadOperation::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case Rendering::LoadOperation::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        default: throw;
        }
    }

    template<>
    inline VkAttachmentStoreOp Convert<Rendering::StoreOperation, VkAttachmentStoreOp>(const Rendering::StoreOperation& value)
    {
        switch (value)
        {
        case Rendering::StoreOperation::Store: return VK_ATTACHMENT_STORE_OP_STORE;
        case Rendering::StoreOperation::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        default: throw;
        }
    }


    template<>
    inline VkShaderStageFlags Convert<ShaderStageFlags, VkShaderStageFlags>(const ShaderStageFlags& value)
    {
        VkShaderStageFlags result = 0;
        const uint32_t val = value;
        if (val & (uint32_t)ShaderStageFlagBits::Vertex)
            result |= VK_SHADER_STAGE_VERTEX_BIT;
        if (val & (uint32_t)ShaderStageFlagBits::Fragment)
            result |= VK_SHADER_STAGE_FRAGMENT_BIT;
        if (val & (uint32_t)ShaderStageFlagBits::Geometry)
            result |= VK_SHADER_STAGE_GEOMETRY_BIT;
        if (val & (uint32_t)ShaderStageFlagBits::Tessellation)
            result |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        if (val & (uint32_t)ShaderStageFlagBits::Mesh)
            result |= VK_SHADER_STAGE_MESH_BIT_EXT;
        if (val & (uint32_t)ShaderStageFlagBits::Compute)
            result |= VK_SHADER_STAGE_COMPUTE_BIT;
        if (val & (uint32_t)ShaderStageFlagBits::RayGeneration)
            result |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        return result;
    }
}
