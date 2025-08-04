#pragma once
#include "Rendering/ShaderBindingSetLayout.h"


typedef struct VkDescriptorSet_T* VkDescriptorSet;
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;
typedef struct VkPipelineLayout_T* VkPipelineLayout;

namespace Nova::Vulkan
{
    class Device;

    class ShaderBindingSetLayout : public Rendering::ShaderBindingSetLayout
    {
    public:
        bool Initialize(const Rendering::ShaderBindingSetLayoutCreateInfo& createInfo) override;
        void Destroy() override;
        bool Build() override;

        const VkDescriptorSetLayout& GetDescriptorSetLayout() const;
    private:
        Device* m_Device = nullptr;
        VkDescriptorSetLayout m_DescriptorSetLayout = nullptr;
    };
}


