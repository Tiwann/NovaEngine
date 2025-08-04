#include "ShaderBindingSetLayout.h"
#include "Device.h"
#include "Conversions.h"
#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    bool ShaderBindingSetLayout::Build()
    {
        Array<VkDescriptorSetLayoutBinding> bindings;

        for (const auto& [index, binding] : m_Bindings)
        {
            VkDescriptorSetLayoutBinding vkBinding;
            vkBinding.binding = index;
            vkBinding.descriptorCount = 1;
            vkBinding.stageFlags = Convert<ShaderStageFlags, VkShaderStageFlags>(binding.stageFlags);
            vkBinding.descriptorType = Convert<ResourceBindingType, VkDescriptorType>(binding.bindingType);
            bindings.Add(vkBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        layoutCreateInfo.bindingCount = bindings.Size();
        layoutCreateInfo.pBindings = bindings.Data();

        const VkDevice deviceHandle = m_Device->GetHandle();
        if (vkCreateDescriptorSetLayout(deviceHandle, &layoutCreateInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
            return false;

        return true;
    }

    bool ShaderBindingSetLayout::Initialize(const Rendering::ShaderBindingSetLayoutCreateInfo& createInfo)
    {
        m_Device = (Device*)createInfo.device;
        return true;
    }

    void ShaderBindingSetLayout::Destroy()
    {
        const VkDevice deviceHandle = m_Device->GetHandle();
        vkDestroyDescriptorSetLayout(deviceHandle, m_DescriptorSetLayout, nullptr);
    }

    const VkDescriptorSetLayout& ShaderBindingSetLayout::GetDescriptorSetLayout() const
    {
        return m_DescriptorSetLayout;
    }
}
