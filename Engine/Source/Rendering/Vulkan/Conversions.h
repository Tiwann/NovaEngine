#pragma once
#include "Rendering/BlendFactor.h"
#include "Rendering/BlendOperation.h"
#include "Rendering/CompareOperation.h"
#include "Rendering/CullMode.h"
#include "Rendering/BindingType.h"
#include "Rendering/FrontFace.h"
#include "Rendering/PolygonMode.h"
#include "Rendering/PrimitiveTopology.h"
#include "Rendering/SamplerAddressMode.h"
#include "Rendering/PresentMode.h"
#include "Rendering/Filter.h"
#include "Runtime/Format.h"
#include "Rendering/ShaderStage.h"
#include "Rendering/LoadOperation.h"
#include "Rendering/ResolveMode.h"
#include "Rendering/StoreOperation.h"
#include "Rendering/ResourceState.h"
#include "Rendering/ResourceBarrier.h"
#include "Rendering/TextureUsage.h"
#include "Rendering/TextureAspect.h"
#include "Rendering/TextureDimension.h"
#include "Rendering/RenderPass.h"

#include <vulkan/vulkan.h>


namespace Nova::Vulkan
{
    template<typename VulkanType, typename NovaType>
    VulkanType Convert(const NovaType& value);

    template<>
    inline VkFormat Convert(const Format& value)
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
        case Format::R64G64B64A64_FLOAT:    return VK_FORMAT_R64G64B64A64_SFLOAT;
        case Format::D32_FLOAT: return VK_FORMAT_D32_SFLOAT;
        case Format::D32_FLOAT_S8_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case Format::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;

        // Compressed formats
        case Format::BC1_UNORM: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case Format::BC1_SRGB: return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
        case Format::BC2_UNORM: return VK_FORMAT_BC2_UNORM_BLOCK;
        case Format::BC2_SRGB: return VK_FORMAT_BC2_SRGB_BLOCK;
        case Format::BC3_UNORM: return VK_FORMAT_BC3_UNORM_BLOCK;
        case Format::BC3_SRGB: return VK_FORMAT_BC3_SRGB_BLOCK;
        case Format::BC4_UNORM: return VK_FORMAT_BC4_UNORM_BLOCK;
        case Format::BC4_SNORM: return VK_FORMAT_BC4_SNORM_BLOCK;
        case Format::BC5_UNORM: return VK_FORMAT_BC5_UNORM_BLOCK;
        case Format::BC5_SNORM: return VK_FORMAT_BC5_SNORM_BLOCK;
        case Format::BC7_UNORM: return VK_FORMAT_BC7_UNORM_BLOCK;
        case Format::BC7_SRGB: return VK_FORMAT_BC7_SRGB_BLOCK;

        // ASTC formats

        case Format::ASTC_4X4_UNORM: return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        case Format::ASTC_4X4_FLOAT: return VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK;
        case Format::ASTC_4X4_SRGB: return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;

        case Format::ASTC_5X4_UNORM: return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
        case Format::ASTC_5X4_FLOAT: return VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK;
        case Format::ASTC_5X4_SRGB: return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;

        case Format::ASTC_5X5_UNORM: return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
        case Format::ASTC_5X5_FLOAT: return VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK;
        case Format::ASTC_5X5_SRGB: return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;

        case Format::ASTC_6X5_UNORM: return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
        case Format::ASTC_6X5_FLOAT: return VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK;
        case Format::ASTC_6X5_SRGB: return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;

        case Format::ASTC_6X6_UNORM: return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
        case Format::ASTC_6X6_FLOAT: return VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK;
        case Format::ASTC_6X6_SRGB: return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;

        case Format::ASTC_8X5_UNORM: return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
        case Format::ASTC_8X5_FLOAT: return VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK;
        case Format::ASTC_8X5_SRGB: return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;

        case Format::ASTC_8X6_UNORM: return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
        case Format::ASTC_8X6_FLOAT: return VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK;
        case Format::ASTC_8X6_SRGB: return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;

        case Format::ASTC_8X8_UNORM: return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
        case Format::ASTC_8X8_FLOAT: return VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK;
        case Format::ASTC_8X8_SRGB: return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;

        case Format::ASTC_10X5_UNORM: return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
        case Format::ASTC_10X5_FLOAT: return VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK;
        case Format::ASTC_10X5_SRGB: return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;

        case Format::ASTC_10X6_UNORM: return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
        case Format::ASTC_10X6_FLOAT: return VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK;
        case Format::ASTC_10X6_SRGB: return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;

        case Format::ASTC_10X8_UNORM: return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
        case Format::ASTC_10X8_FLOAT: return VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK;
        case Format::ASTC_10X8_SRGB: return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;

        case Format::ASTC_10X10_UNORM: return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
        case Format::ASTC_10X10_FLOAT: return VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK;
        case Format::ASTC_10X10_SRGB: return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;

        case Format::ASTC_12X10_UNORM: return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
        case Format::ASTC_12X10_FLOAT: return VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK;
        case Format::ASTC_12X10_SRGB: return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;

        case Format::ASTC_12X12_UNORM: return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
        case Format::ASTC_12X12_FLOAT: return VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK;
        case Format::ASTC_12X12_SRGB: return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
        default: return VK_FORMAT_UNDEFINED;
        }
    }

    template<>
    inline VkPresentModeKHR Convert(const PresentMode& value)
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
    inline VkFilter Convert(const Filter& filter)
    {
        switch (filter)
        {
        case Filter::Nearest: return VK_FILTER_NEAREST;
        case Filter::Linear: return VK_FILTER_LINEAR;
        default: return VK_FILTER_NEAREST;
        }
    }

    template<>
    inline VkPrimitiveTopology Convert(const PrimitiveTopology& value)
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
    inline VkCullModeFlags Convert(const CullMode& value)
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
    inline VkFrontFace Convert(const FrontFace& value)
    {
        switch (value)
        {
        case FrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
        case FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        default: throw;
        }
    }

    template<>
    inline VkPolygonMode Convert(const PolygonMode& value)
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
    inline VkCompareOp Convert(const CompareOperation& value)
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
    inline VkSamplerAddressMode Convert(const SamplerAddressMode& value)
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
    inline VkBlendOp Convert(const BlendOperation& value)
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
    inline VkBlendFactor Convert(const BlendFactor& value)
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
    inline VkIndexType Convert(const Format& value)
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
    inline VkDescriptorType Convert(const BindingType& value)
    {
        switch (value)
        {
        case BindingType::Sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
        case BindingType::CombinedTextureSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case BindingType::SampledTexture: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case BindingType::StorageTexture: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case BindingType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case BindingType::StorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case BindingType::InputAttachment: return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        case BindingType::InlineUniformBlock: return VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK;
        case BindingType::AccelerationStructure: return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        case BindingType::StorageTexelBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        case BindingType::UniformTexelBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        default: throw;
        }
    }

    template<>
    inline VkSamplerMipmapMode Convert(const Filter& value)
    {
        switch (value)
        {
        case Filter::Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case Filter::Linear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        default: throw;
        }
    }

    template<>
    inline VkAttachmentLoadOp Convert(const LoadOperation& value)
    {
        switch (value)
        {
        case LoadOperation::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
        case LoadOperation::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case LoadOperation::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        default: throw;
        }
    }

    template<>
    inline VkAttachmentStoreOp Convert(const StoreOperation& value)
    {
        switch (value)
        {
        case StoreOperation::Store: return VK_ATTACHMENT_STORE_OP_STORE;
        case StoreOperation::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        default: throw;
        }
    }


    template<>
    inline VkShaderStageFlags Convert(const ShaderStageFlags& value)
    {
        VkShaderStageFlags result = 0;
        const uint32_t val = value;
        if (val & (uint32_t)ShaderStageFlagBits::Vertex)
            result |= VK_SHADER_STAGE_VERTEX_BIT;
        if (val & (uint32_t)ShaderStageFlagBits::Fragment)
            result |= VK_SHADER_STAGE_FRAGMENT_BIT;
        if (val & (uint32_t)ShaderStageFlagBits::Geometry)
            result |= VK_SHADER_STAGE_GEOMETRY_BIT;
        if (val & (uint32_t)ShaderStageFlagBits::TessellationControl)
            result |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        if (val & (uint32_t)ShaderStageFlagBits::Mesh)
            result |= VK_SHADER_STAGE_MESH_BIT_EXT;
        if (val & (uint32_t)ShaderStageFlagBits::Compute)
            result |= VK_SHADER_STAGE_COMPUTE_BIT;
        if (val & (uint32_t)ShaderStageFlagBits::RayGeneration)
            result |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        return result;
    }

    template<>
    inline VkShaderStageFlagBits Convert(const ShaderStageFlagBits& value)
    {
        switch (value)
        {
        case ShaderStageFlagBits::None: return (VkShaderStageFlagBits)0;
        case ShaderStageFlagBits::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStageFlagBits::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStageFlagBits::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStageFlagBits::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
        case ShaderStageFlagBits::RayGeneration: return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        case ShaderStageFlagBits::TessellationControl: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case ShaderStageFlagBits::Mesh: return VK_SHADER_STAGE_MESH_BIT_EXT;
        default: return (VkShaderStageFlagBits)0;
        }
    }

    template<>
    inline VkResolveModeFlagBits Convert(const ResolveMode& value)
    {
        switch (value)
        {
        case ResolveMode::None: return VK_RESOLVE_MODE_NONE;
        case ResolveMode::Zero: return VK_RESOLVE_MODE_SAMPLE_ZERO_BIT;
        case ResolveMode::Average: return VK_RESOLVE_MODE_AVERAGE_BIT;
        case ResolveMode::Min: return VK_RESOLVE_MODE_MIN_BIT;
        case ResolveMode::Max: return VK_RESOLVE_MODE_MAX_BIT;
        default: return VK_RESOLVE_MODE_NONE;
        }
    }

    template<>
    inline VkAccessFlags Convert(const AccessFlags& value)
    {
        VkAccessFlags result = VK_ACCESS_NONE;
        if (value == AccessFlagBits::None)
            return VK_ACCESS_NONE;

        if (value & AccessFlagBits::ShaderRead)
            result |= VK_ACCESS_SHADER_READ_BIT;
        if (value & AccessFlagBits::ShaderWrite)
            result |= VK_ACCESS_SHADER_WRITE_BIT;
        if (value & AccessFlagBits::ColorAttachmentRead)
            result |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        if (value & AccessFlagBits::ColorAttachmentWrite)
            result |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        if (value & AccessFlagBits::DepthStencilAttachmentRead)
            result |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        if (value & AccessFlagBits::DepthStencilAttachmentWrite)
            result |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        if (value & AccessFlagBits::TransferRead)
            result |= VK_ACCESS_TRANSFER_READ_BIT;
        if (value & AccessFlagBits::TransferWrite)
            result |= VK_ACCESS_TRANSFER_WRITE_BIT;
        if (value & AccessFlagBits::HostRead)
            result |= VK_ACCESS_HOST_READ_BIT;
        if (value & AccessFlagBits::HostWrite)
            result |= VK_ACCESS_HOST_WRITE_BIT;

        return result;
    }

    template<>
    inline VkImageLayout Convert(const ResourceState& value)
    {
        switch (value)
        {
        case ResourceState::Undefined: return VK_IMAGE_LAYOUT_UNDEFINED;
        case ResourceState::General: return VK_IMAGE_LAYOUT_GENERAL;
        case ResourceState::ShaderRead: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case ResourceState::ShaderWrite: return VK_IMAGE_LAYOUT_GENERAL;
        case ResourceState::ColorAttachment: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case ResourceState::DepthStencilAttachment: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case ResourceState::TransferSource: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case ResourceState::TransferDest: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case ResourceState::Present: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        default: return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }

    template<>
    inline VkImageUsageFlags Convert(const TextureUsageFlags& value)
    {
        VkImageUsageFlags flags = 0;
        if (value == TextureUsageFlagBits::None)
            return 0;

        if (value & TextureUsageFlagBits::Sampled)
            flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if (value & TextureUsageFlagBits::Storage)
            flags |= VK_IMAGE_USAGE_STORAGE_BIT;
        if (value & TextureUsageFlagBits::Transfer)
            flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        return flags;
    }

    template<>
    inline VkImageAspectFlags Convert(const TextureAspectFlags& value)
    {
        VkImageAspectFlags flags = 0;
        if (value == TextureAspectFlagBits::None)
            return VK_IMAGE_ASPECT_NONE;
        if (value & TextureAspectFlagBits::Color)
            flags |= VK_IMAGE_ASPECT_COLOR_BIT;
        if (value & TextureAspectFlagBits::Depth)
            flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
        if (value & TextureAspectFlagBits::Stencil)
            flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
        return flags;
    }

    inline VkImageAspectFlags Convert(const Format& value)
    {
        VkImageAspectFlags flags = 0;
        switch (value)
        {
        case Format::None: return 0;
        case Format::R8_UNORM:
        case Format::R8_SNORM:
        case Format::R16_USHORT:
        case Format::R16_SHORT:
        case Format::R32_FLOAT:
        case Format::R32_UINT:
        case Format::R32_SINT:
        case Format::R8G8_UNORM:
        case Format::R8G8_SNORM:
        case Format::R16G16_USHORT:
        case Format::R16G16_SHORT:
        case Format::R32G32_UINT:
        case Format::R32G32_SINT:
        case Format::R32G32_FLOAT:
        case Format::R8G8B8_UNORM:
        case Format::R8G8B8_SNORM:
        case Format::R16G16B16_USHORT:
        case Format::R16G16B16_SHORT:
        case Format::R32G32B32_UINT:
        case Format::R32G32B32_SINT:
        case Format::R32G32B32_FLOAT:
        case Format::R8G8B8A8_UNORM:
        case Format::R8G8B8A8_SNORM:
        case Format::R8G8B8A8_SRGB:
        case Format::R16G16B16A16_USHORT:
        case Format::R16G16B16A16_SHORT:
        case Format::R32G32B32A32_UINT:
        case Format::R32G32B32A32_SINT:
        case Format::R32G32B32A32_FLOAT:
        case Format::R64G64B64A64_FLOAT:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        case Format::D32_FLOAT:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        case Format::D32_FLOAT_S8_UINT:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        case Format::D24_UNORM_S8_UINT:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        default: return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

    template<>
    inline VkImageLayout Convert(const RenderPassAttachmentType& value)
    {
        switch (value)
        {
        case RenderPassAttachmentType::Color: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case RenderPassAttachmentType::Depth: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        default: return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }

    template<>
    inline VkImageType Convert(const TextureDimension& value)
    {
        switch (value)
        {
        case TextureDimension::None: return VK_IMAGE_TYPE_MAX_ENUM;
        case TextureDimension::Dim1D: return VK_IMAGE_TYPE_1D;
        case TextureDimension::Dim2D: return VK_IMAGE_TYPE_2D;
        case TextureDimension::Dim3D: return VK_IMAGE_TYPE_3D;
        default: return VK_IMAGE_TYPE_MAX_ENUM;
        }
    }
}
