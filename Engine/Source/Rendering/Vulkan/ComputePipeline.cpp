#include "ComputePipeline.h"
#include "Device.h"
#include "ShaderModule.h"

#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    bool ComputePipeline::Initialize(const Rendering::ComputePipelineCreateInfo& createInfo)
    {
        Device* device = (Device*)createInfo.device;
        const ShaderModule* shaderModule = (ShaderModule*)createInfo.shaderModule;

        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        shaderStageCreateInfo.pName = "main";
        shaderStageCreateInfo.module = shaderModule->GetHandle();

        VkComputePipelineCreateInfo computeCreateInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
        computeCreateInfo.layout = (VkPipelineLayout)createInfo.pipelineLayout;
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
