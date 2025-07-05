#pragma once
#include "DescriptorSetInfo.h"
#include "Rendering/DescriptorPool.h"

typedef VkDescriptorPool_T* VkDescriptorPool;

namespace Nova
{
    class VulkanDescriptorPool : public DescriptorPool
    {
    public:
        explicit VulkanDescriptorPool(Renderer* Owner) : DescriptorPool("Descriptor Pool", Owner)
        {
        }

        bool Initialize(const DescriptorPoolCreateInfo& CreateInfo) override;
        void SetDebugName(const String& Name) override;
        void Destroy() override;

        VkDescriptorPool GetHandle() const;
        const VkDescriptorPool* GetHandlePtr() const;
    private:
        VkDescriptorPool m_Handle = nullptr;
    };
}
