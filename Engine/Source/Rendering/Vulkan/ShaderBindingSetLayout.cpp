#include "ShaderBindingSetLayout.h"
#include "Device.h"
#include "Conversions.h"
#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    bool ShaderBindingSetLayout::Build()
    {
        Array<VkDescriptorSetLayoutBinding> bindings;

        for (size_t i = 0; i < m_Bindings.Count(); i++)
        {
            const ShaderBinding& binding = m_Bindings[i];
            VkDescriptorSetLayoutBinding vkBinding = { };
            vkBinding.binding = i;
            vkBinding.descriptorCount = 1;
            vkBinding.stageFlags = Convert<ShaderStageFlags, VkShaderStageFlags>(binding.stageFlags);
            vkBinding.descriptorType = Convert<BindingType, VkDescriptorType>(binding.bindingType);
            vkBinding.pImmutableSamplers = nullptr;
            bindings.Add(vkBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        layoutCreateInfo.bindingCount = bindings.Count();
        layoutCreateInfo.pBindings = bindings.Data();

        const VkDevice deviceHandle = m_Device->GetHandle();

        vkDestroyDescriptorSetLayout(deviceHandle, m_Handle, nullptr);
        if (vkCreateDescriptorSetLayout(deviceHandle, &layoutCreateInfo, nullptr, &m_Handle) != VK_SUCCESS)
            return false;

        return true;
    }

    bool ShaderBindingSetLayout::Initialize(Nova::Device* device, uint32_t setIndex)
    {
        m_Device = (Device*)device;
        m_SetIndex = setIndex;
        return true;
    }

    void ShaderBindingSetLayout::Destroy()
    {
        const VkDevice deviceHandle = m_Device->GetHandle();
        vkDestroyDescriptorSetLayout(deviceHandle, m_Handle, nullptr);
    }

    const VkDescriptorSetLayout& ShaderBindingSetLayout::GetHandle() const
    {
        return m_Handle;
    }
}
