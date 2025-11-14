#pragma once
#include "BindingType.h"
#include "Containers/Map.h"

namespace Nova::Rendering
{
    class Device;
    struct DescriptorPoolCreateInfo
    {
        Device* device;
        Map<BindingType, uint32_t> sizes;
        size_t maxSets;

        DescriptorPoolCreateInfo& SetDevice(Device* device) { this->device = device; return *this; }
        DescriptorPoolCreateInfo& SetBindingTypeSize(const BindingType bindingType, const uint32_t size) { sizes[bindingType] = size; return *this; }
        DescriptorPoolCreateInfo& SetMaxSets(const uint32_t maxSets) { this->maxSets = maxSets; return *this; }
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
