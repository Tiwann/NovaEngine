#include "ShaderModule.h"
#include "RenderDevice.h"

#include <vulkan/vulkan.h>


namespace Nova::Vulkan
{
    bool ShaderModule::Initialize(const ShaderModuleCreateInfo& createInfo)
    {
        RenderDevice* device = (RenderDevice*)createInfo.device;

        VkShaderModuleCreateInfo shaderModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        shaderModuleCreateInfo.codeSize = createInfo.codeSize;
        shaderModuleCreateInfo.pCode = createInfo.code;

        const VkDevice deviceHandle = device->GetHandle();
        vkDestroyShaderModule(deviceHandle, m_Handle, nullptr);
        const VkResult result = vkCreateShaderModule(deviceHandle, &shaderModuleCreateInfo, nullptr, &m_Handle);
        if (result != VK_SUCCESS)
            return false;

        m_Device = device;
        m_Stage = createInfo.stage;
        return true;
    }

    void ShaderModule::Destroy()
    {
        const VkDevice deviceHandle = m_Device->GetHandle();
        vkDestroyShaderModule(deviceHandle, m_Handle, nullptr);
    }

    VkShaderModule ShaderModule::GetHandle() const
    {
        return m_Handle;
    }

    /*VkPipelineShaderStageCreateInfo ShaderModule::GetShaderStageCreateInfo() const
    {
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        shaderStageCreateInfo.module = m_Handle;
        shaderStageCreateInfo.pName = "main";
        shaderStageCreateInfo.stage = Convert<ShaderStageFlagBits, VkShaderStageFlagBits>(m_Stage);
        return shaderStageCreateInfo;
    }*/
}
