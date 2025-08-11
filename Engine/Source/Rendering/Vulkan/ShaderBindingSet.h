#pragma once
#include "Rendering/ShaderBindingSet.h"

typedef struct VkDescriptorSet_T* VkDescriptorSet;

namespace Nova::Vulkan
{
    class ShaderBindingSet : public Rendering::ShaderBindingSet
    {
    public:
        bool Initialize(const Rendering::DescriptorPool& descriptorPool, const Rendering::ShaderBindingSetLayout& bindingSetLayout) override;
        void Destroy() override;

        VkDescriptorSet GetHandle() const;
    private:
        VkDescriptorSet m_Handle = nullptr;
    };
}
