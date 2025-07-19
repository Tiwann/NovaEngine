#include "ShaderModule.h"
#include "Device.h"

#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    ShaderModule::ShaderModule() : Rendering::ShaderModule()
    {

    }

    ShaderModule::ShaderModule(ShaderModule&& other) noexcept
    {
        if (this == &other)
            return;

        m_Device = other.m_Device;
        m_Handle = other.m_Handle;
        m_Stage = other.m_Stage;
    }

    ShaderModule& ShaderModule::operator=(ShaderModule&& other) noexcept
    {
        if (this == &other)
            return *this;

        m_Device = other.m_Device;
        m_Handle = other.m_Handle;
        m_Stage = other.m_Stage;

        other.m_Device = nullptr;
        other.m_Handle = nullptr;
        other.m_Stage = ShaderStageFlagBits::None;
        return *this;
    }

    bool ShaderModule::Initialize(const Rendering::ShaderModuleCreateInfo& createInfo)
    {
        Device* device = (Device*)createInfo.device;

        VkShaderModuleCreateInfo shaderModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        shaderModuleCreateInfo.codeSize = createInfo.codeSize;
        shaderModuleCreateInfo.pCode = createInfo.code;

        const VkDevice deviceHandle = device->GetHandle();
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
}
