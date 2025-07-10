#pragma once
#include "SlangConversions.h"
#include "Containers/Map.h"
#include "Rendering/DescriptorType.h"
#include <vulkan/vulkan.h>
#include <slang/slang.h>

namespace Nova
{


    static Map<slang::BindingType, VkDescriptorType> ToVulkanDescriptorTypeMap
    {
        { slang::BindingType::Sampler, VK_DESCRIPTOR_TYPE_SAMPLER },
        { slang::BindingType::CombinedTextureSampler, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER },
        { slang::BindingType::Texture, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE },
        { slang::BindingType::MutableTexture, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE },
        { slang::BindingType::TypedBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER },
        { slang::BindingType::MutableTypedBuffer, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER },
        { slang::BindingType::RawBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER },
        { slang::BindingType::MutableRawBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER },
        { slang::BindingType::InputRenderTarget, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT },
        { slang::BindingType::InlineUniformData, VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT },
        { slang::BindingType::RayTracingAccelerationStructure, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR },
        { slang::BindingType::ConstantBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
    };

    static Map<slang::BindingType, DescriptorType> ToNovaDescriptorTypeMap
    {
            { slang::BindingType::Sampler, DescriptorType::Sampler },
            { slang::BindingType::CombinedTextureSampler, DescriptorType::CombinedImageSampler },
            { slang::BindingType::Texture, DescriptorType::SampledImage },
            { slang::BindingType::MutableTexture, DescriptorType::StorageImage },
            { slang::BindingType::TypedBuffer, DescriptorType::UniformTexelBuffer },
            { slang::BindingType::MutableTypedBuffer, DescriptorType::StorageTexelBuffer },
            { slang::BindingType::RawBuffer, DescriptorType::StorageBuffer },
            { slang::BindingType::MutableRawBuffer, DescriptorType::StorageBuffer },
            { slang::BindingType::InputRenderTarget, DescriptorType::InputAttachment },
            { slang::BindingType::InlineUniformData, DescriptorType::InlineUniformBlock },
            { slang::BindingType::RayTracingAccelerationStructure, DescriptorType::AccelerationStructure },
            { slang::BindingType::ConstantBuffer, DescriptorType::UniformBuffer },
        };

    static Map<SlangStage, VkShaderStageFlags> ToVulkanSlangStage
    {
        { SLANG_STAGE_NONE, 0 },
        { SLANG_STAGE_VERTEX, VK_SHADER_STAGE_VERTEX_BIT },
        { SLANG_STAGE_GEOMETRY, VK_SHADER_STAGE_GEOMETRY_BIT },
        { SLANG_STAGE_FRAGMENT, VK_SHADER_STAGE_FRAGMENT_BIT },
        { SLANG_STAGE_COMPUTE, VK_SHADER_STAGE_COMPUTE_BIT },
        { SLANG_STAGE_RAY_GENERATION, VK_SHADER_STAGE_RAYGEN_BIT_KHR },
        { SLANG_STAGE_INTERSECTION, VK_SHADER_STAGE_INTERSECTION_BIT_KHR },
        { SLANG_STAGE_ANY_HIT, VK_SHADER_STAGE_ANY_HIT_BIT_KHR },
        { SLANG_STAGE_CLOSEST_HIT, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR },
        { SLANG_STAGE_MISS, VK_SHADER_STAGE_MISS_BIT_KHR },
        { SLANG_STAGE_CALLABLE, VK_SHADER_STAGE_CALLABLE_BIT_KHR },
        { SLANG_STAGE_MESH, VK_SHADER_STAGE_MESH_BIT_EXT },
    };

    VkDescriptorType ToVulkanDescriptorType(const slang::BindingType BindingType)
    {
        return ToVulkanDescriptorTypeMap[BindingType];
    }

    VkShaderStageFlags ToVulkanShaderStage(const SlangStage Stage)
    {
        return ToVulkanSlangStage[Stage];
    }

    DescriptorType ToNovaDescriptorType(const slang::BindingType BindingType)
    {
        return ToNovaDescriptorTypeMap[BindingType];
    }
}
