#pragma once
#include "Rendering/GraphicsPipeline.h"

typedef struct VkPipeline_T* VkPipeline;

namespace Nova::Vulkan
{
    class RenderDevice;

    class GraphicsPipeline final : public Nova::GraphicsPipeline
    {
    public:
        bool Initialize(const GraphicsPipelineCreateInfo& createInfo) override;
        void Destroy() override;

        VkPipeline GetHandle() const;
        const VkPipeline* GetHandlePtr() const;
    private:
        RenderDevice* m_Device = nullptr;
        VkPipeline m_Handle = nullptr;
    };
}
