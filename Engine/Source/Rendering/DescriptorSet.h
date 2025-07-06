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

    class DescriptorSet : public Object
    {
    public:
        virtual bool Initialize(const DescriptorSetAllocateInfo& AllocateInfo) = 0;
        virtual void Destroy() = 0;

        virtual void SetDebugName(const String& Name)
        {
        }

        DescriptorPool* GetDescriptorPool() const { return m_Pool; }

    protected:
        DescriptorPool* m_Pool = nullptr;

    private:
        explicit DescriptorSet(DescriptorPool* Owner);
        friend DescriptorPool;
    };
}
