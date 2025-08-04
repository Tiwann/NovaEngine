#pragma once
#include "ResourceBindingType.h"
#include "Containers/Map.h"

namespace Nova::Rendering
{
    class Device;
    struct DescriptorPoolCreateInfo
    {
        Device* device;
        Map<ResourceBindingType, uint32_t> sizes;
        size_t maxSets;
    };

    class DescriptorPool
    {
    public:
        DescriptorPool() = default;
        virtual ~DescriptorPool() = default;

        virtual bool Initialize(const DescriptorPoolCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
    };
}
