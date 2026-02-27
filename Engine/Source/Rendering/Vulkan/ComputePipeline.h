#pragma once
#include "Rendering/ComputePipeline.h"

typedef struct VkPipeline_T* VkPipeline;

namespace Nova::Vulkan
{
    class RenderDevice;

    class ComputePipeline final : public Nova::ComputePipeline
    {
    public:
        bool Initialize(const ComputePipelineCreateInfo& createInfo) override;
        void Destroy() override;

        VkPipeline GetHandle() const;
        const VkPipeline* GetHandlePtr() const;
    private:
        RenderDevice* m_Device = nullptr;
        VkPipeline m_Handle = nullptr;
    };
}
