﻿#pragma once
#include "Rendering/ShaderBindingSetLayout.h"

typedef struct VkDescriptorSet_T* VkDescriptorSet;
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;

namespace Nova::Vulkan
{
    class Device;

    class ShaderBindingSetLayout : public Rendering::ShaderBindingSetLayout
    {
    public:
        bool Initialize(Rendering::Device* device) override;
        void Destroy() override;
        bool Build() override;

        const VkDescriptorSetLayout& GetHandle() const;
    private:
        Device* m_Device = nullptr;
        VkDescriptorSetLayout m_Handle = nullptr;
    };
}


