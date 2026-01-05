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

#include <vulkan/vulkan.h>


namespace Nova::Vulkan
{
    template<typename VulkanType, typename NovaType>
    VulkanType Convert(const NovaType& value);

    template<>
    VkFormat Convert(const Format& value);

    template<>
    VkPresentModeKHR Convert(const PresentMode& value);

    template<>
    VkFilter Convert(const Filter& filter);

    template<>
    VkPrimitiveTopology Convert(const PrimitiveTopology& value);

    template<>
    VkCullModeFlags Convert(const CullMode& value);

    template<>
    VkFrontFace Convert(const FrontFace& value);

    template<>
    VkPolygonMode Convert(const PolygonMode& value);

    template<>
    VkCompareOp Convert(const CompareOperation& value);

    template<>
    VkSamplerAddressMode Convert(const SamplerAddressMode& value);

    template<>
    VkBlendOp Convert(const BlendOperation& value);

    template<>
    VkBlendFactor Convert(const BlendFactor& value);

    template<>
    VkIndexType Convert(const Format& value);

    template<>
    VkDescriptorType Convert(const BindingType& value);

    template<>
    VkSamplerMipmapMode Convert(const Filter& value);

    template<>
    VkAttachmentLoadOp Convert(const LoadOperation& value);

    template<>
    VkAttachmentStoreOp Convert(const StoreOperation& value);


    template<>
    VkShaderStageFlags Convert(const ShaderStageFlags& value);

    template<>
    VkShaderStageFlagBits Convert(const ShaderStageFlagBits& value);

    template<>
    VkResolveModeFlagBits Convert(const ResolveMode& value);

    template<>
    VkAccessFlags Convert(const AccessFlags& value);

    template<>
    VkImageLayout Convert(const ResourceState& value);

    template<>
    VkImageUsageFlags Convert(const TextureUsageFlags& value);

    template<>
    VkImageAspectFlags Convert(const TextureAspectFlags& value);

    VkImageAspectFlags Convert(const Format& value);

    template<>
    VkImageType Convert(const TextureDimension& value);
}
