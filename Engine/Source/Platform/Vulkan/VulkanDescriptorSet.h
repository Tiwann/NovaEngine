#pragma once
#include "Rendering/DescriptorSet.h"

typedef struct VkDescriptorSet_T* VkDescriptorSet;

namespace Nova
{
    class VulkanDescriptorSet : public DescriptorSet
    {
    public:
        explicit VulkanDescriptorSet(DescriptorPool* Owner);
        bool Initialize(const DescriptorSetAllocateInfo& AllocateInfo) override;
        void Destroy() override;
        void SetDebugName(const String& Name) override;

        VkDescriptorSet GetHandle() const;
        const VkDescriptorSet* GetHandlePtr() const;
    private:
        VkDescriptorSet m_Handle = nullptr;
    };
}
