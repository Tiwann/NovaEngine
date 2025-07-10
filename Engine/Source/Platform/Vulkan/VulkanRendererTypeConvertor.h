#pragma once
#include "Rendering/RendererTypeConvertor.h"
#include <vulkan/vulkan.h>

namespace Nova
{

    struct VulkanRendererTypeConvertor : RendererTypeConvertor<VkCullModeFlags, VkBlendFactor, VkBlendOp,
    VkFilter, VkFrontFace, VkPolygonMode, VkPrimitiveTopology, VkSamplerAddressMode, VkShaderStageFlagBits, VkCompareOp, VkFormat, VkPresentModeKHR>
    {
        VkCullModeFlags ConvertCullMode(const CullMode Mode) const override;
        VkBlendFactor ConvertBlendFactor(const BlendFactor Factor) const override;
        VkBlendOp ConvertBlendOperation(const BlendOperation Operation) const override;
        VkFilter ConvertFilter(const Filter Filter) const override;
        VkFrontFace ConvertFrontFace(const FrontFace Face) const override;
        VkPolygonMode ConvertPolygonMode(const PolygonMode Mode) const override;
        VkPrimitiveTopology ConvertPrimitiveTopology(const PrimitiveTopology Topology) const override;
        VkSamplerAddressMode ConvertSamplerAddressMode(const SamplerAddressMode Address) const override;
        VkShaderStageFlagBits ConvertShaderStage(const ShaderStageFlagBits Stage) const override;
        VkCompareOp ConvertCompareOperation(const CompareOperation Operation) const override;
        VkFormat ConvertFormat(const Format Format) const override;
        VkPresentModeKHR ConvertPresentMode(const PresentMode PresentMode) const override;
    };
}
