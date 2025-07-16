#pragma once
#include "Rendering/GraphicsPipeline.h"

typedef struct VkPipeline_T* VkPipeline;

namespace Nova::Vulkan
{
    class Device;

    class GraphicsPipeline : public Rendering::GraphicsPipeline
    {
    public:
        bool Initialize(const Rendering::GraphicsPipelineCreateInfo& createInfo) override;
        void Destroy() override;

        VkPipeline GetHandle() const;
        const VkPipeline* GetHandlePtr() const;
    private:
        Device* m_Device = nullptr;
        VkPipeline m_Handle = nullptr;
    };
}
