#pragma once
#include "Rendering/DescriptorPool.h"

typedef struct VkDescriptorPool_T* VkDescriptorPool;
typedef struct VkDescriptorSet_T* VkDescriptorSet;

namespace Nova { class ShaderBindingSetLayout; }

namespace Nova::Vulkan
{
    class RenderDevice;

    class DescriptorPool final : public Nova::DescriptorPool
    {
    public:
        bool Initialize(const DescriptorPoolCreateInfo& createInfo) override;
        void Destroy() override;

        VkDescriptorPool GetHandle() const;
        const VkDescriptorPool* GetHandlePtr() const;

        VkDescriptorSet AllocateDescriptorSet(const ShaderBindingSetLayout& bindingSetLayout) const;
    private:
        RenderDevice* m_Device = nullptr;
        VkDescriptorPool m_Handle = nullptr;
    };
}
