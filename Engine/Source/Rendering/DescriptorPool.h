#pragma once
#include "DescriptorType.h"
#include "RendererObject.h"
#include "Containers/Map.h"

namespace Nova
{
    struct DescriptorPoolCreateInfo
    {
        Map<DescriptorType, u32> PoolSizes;
        size_t MaxDescriptorSets;
    };

    class DescriptorPool : public RendererObject<DescriptorPoolCreateInfo>
    {
    public:
        explicit DescriptorPool(const String& Name, Renderer* Owner) : RendererObject(Name, Owner)
        {
        }

        class DescriptorSet* AllocateDescriptorSet(const struct DescriptorSetAllocateInfo& AllocateInfo);
        void FreeDescriptorSet(DescriptorSet* DescriptorSet);
    };
}
