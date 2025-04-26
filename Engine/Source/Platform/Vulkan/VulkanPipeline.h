#pragma once
#include "Rendering/Pipeline.h"
#include <vulkan/vulkan.h>


namespace Nova
{
    class VulkanPipeline : public Pipeline
    {
    public:
        explicit VulkanPipeline(Renderer* Renderer);
        ~VulkanPipeline() override = default;
        VkPipeline GetHandle() const;

        bool Initialize(const PipelineSpecification& Specification) override;
        void Destroy() override;

    private:
        VkPipeline m_Handle;
    };
}
