#pragma once
#include "Rendering/DescriptorPool.h"

typedef struct VkDescriptorPool_T* VkDescriptorPool;

namespace Nova::Vulkan
{
    class Device;

    class DescriptorPool : public Rendering::DescriptorPool
    {
    public:
        bool Initialize(const Rendering::DescriptorPoolCreateInfo& createInfo) override;
        void Destroy() override;

        VkDescriptorPool GetHandle() const;
        const VkDescriptorPool* GetHandlePtr() const;
    private:
        Device* m_Device = nullptr;
        VkDescriptorPool m_Handle = nullptr;
    };
}
