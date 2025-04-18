#pragma once
#include "SlangConversions.h"

namespace Nova
{
    VkDescriptorType ConvertSlangDescriptorType(const slang::BindingType BindingType)
    {
        switch (BindingType)
        {
        case slang::BindingType::PushConstant:
        default: throw;
        case slang::BindingType::Sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
        case slang::BindingType::CombinedTextureSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case slang::BindingType::Texture: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case slang::BindingType::MutableTexture: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case slang::BindingType::TypedBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        case slang::BindingType::MutableTypedBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        case slang::BindingType::RawBuffer:
        case slang::BindingType::MutableRawBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case slang::BindingType::InputRenderTarget: return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        case slang::BindingType::InlineUniformData: return VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT;
        case slang::BindingType::RayTracingAccelerationStructure: return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        case slang::BindingType::ConstantBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    }

    VkShaderStageFlags ConvertSlangShaderStage(const SlangStage Stage)
    {
        switch (Stage)
        {
        case SLANG_STAGE_NONE: throw;
        case SLANG_STAGE_VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
        case SLANG_STAGE_HULL: throw;
        case SLANG_STAGE_DOMAIN: throw;
        case SLANG_STAGE_GEOMETRY: return VK_SHADER_STAGE_GEOMETRY_BIT;
        case SLANG_STAGE_FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case SLANG_STAGE_COMPUTE: return VK_SHADER_STAGE_COMPUTE_BIT;
        case SLANG_STAGE_RAY_GENERATION: return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        case SLANG_STAGE_INTERSECTION: return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
        case SLANG_STAGE_ANY_HIT: return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
        case SLANG_STAGE_CLOSEST_HIT: return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        case SLANG_STAGE_MISS: return VK_SHADER_STAGE_MISS_BIT_KHR;
        case SLANG_STAGE_CALLABLE: return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
        case SLANG_STAGE_MESH: return VK_SHADER_STAGE_MESH_BIT_EXT;
        case SLANG_STAGE_AMPLIFICATION: throw;
        default: throw;
        }
    }
}
