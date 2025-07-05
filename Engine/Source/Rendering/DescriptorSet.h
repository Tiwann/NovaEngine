#pragma once
#include "RendererObject.h"

namespace Nova
{
    struct DescriptorSetAllocateInfo
    {
        size_t Count;
        const class DescriptorSetLayout& Layout;
    };

    class DescriptorPool;

    class DescriptorSet : public RendererObject<DescriptorSetAllocateInfo>
    {
    public:
        DescriptorPool* GetDescriptorPool() const { return m_Pool; }
    private:
        explicit DescriptorSet(DescriptorPool* Owner);
        friend DescriptorPool;
        DescriptorPool* m_Pool = nullptr;
    };
}
