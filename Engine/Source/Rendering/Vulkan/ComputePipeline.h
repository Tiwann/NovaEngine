#pragma once
#include "Rendering/ComputePipeline.h"

typedef struct VkPipeline_T* VkPipeline;

namespace Nova::Vulkan
{
    class Device;

    class ComputePipeline : public Rendering::ComputePipeline
    {
    public:
        bool Initialize(const Rendering::ComputePipelineCreateInfo& createInfo) override;
        void Destroy() override;

        VkPipeline GetHandle() const;
        const VkPipeline* GetHandlePtr() const;
    private:
        Device* m_Device = nullptr;
        VkPipeline m_Handle = nullptr;
    };
}
