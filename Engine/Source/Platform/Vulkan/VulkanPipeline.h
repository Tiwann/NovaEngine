#pragma once
#include "Rendering/Pipeline.h"
#include <vulkan/vulkan.h>


namespace Nova
{
    class VulkanPipeline : public Pipeline
    {
    public:
        VulkanPipeline(Renderer* Renderer, const PipelineSpecification& Specification);

        VkPipeline GetHandle() const;
    private:
        VkPipeline m_Handle;
    };
}
