#include "ComputePipeline.h"
#include "Device.h"
#include "ShaderModule.h"

#include <vulkan/vulkan.h>

#include "Shader.h"

namespace Nova::Vulkan
{
    bool ComputePipeline::Initialize(const ComputePipelineCreateInfo& createInfo)
    {
        if (!createInfo.device)
            return false;

        if (!createInfo.shader)
            return false;

        const Shader* shader = (Shader*)createInfo.shader;
        const auto stageFlags = shader->GetShaderStageFlags();
        if (!stageFlags.Contains(ShaderStageFlagBits::Compute))
            return false;

        Device* device = (Device*)createInfo.device;

        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        shaderStageCreateInfo.pName = "main";
        shaderStageCreateInfo.module = shader->GetShaderModule(ShaderStageFlagBits::Compute).GetHandle();

        VkComputePipelineCreateInfo computeCreateInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
        computeCreateInfo.layout = shader->GetPipelineLayout();
        computeCreateInfo.stage = shaderStageCreateInfo;
        computeCreateInfo.basePipelineHandle = nullptr;

        const VkDevice deviceHandle = device->GetHandle();
        const VkResult result = vkCreateComputePipelines(deviceHandle, nullptr, 1, &computeCreateInfo, nullptr, &m_Handle);
        if (result != VK_SUCCESS)
            return false;

        m_Device = device;
        return true;
    }

    void ComputePipeline::Destroy()
    {
        vkDestroyPipeline(m_Device->GetHandle(), m_Handle, nullptr);
    }

    VkPipeline ComputePipeline::GetHandle() const
    {
        return m_Handle;
    }

    const VkPipeline* ComputePipeline::GetHandlePtr() const
    {
        return &m_Handle;
    }
}
