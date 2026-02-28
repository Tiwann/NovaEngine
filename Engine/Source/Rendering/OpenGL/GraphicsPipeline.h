#pragma once
#include "Rendering/GraphicsPipeline.h"

namespace Nova::OpenGL
{
    class GraphicsPipeline final : public Nova::GraphicsPipeline
    {
    public:
        bool Initialize(const GraphicsPipelineCreateInfo& createInfo) override;
        void Destroy() override;
    private:
    };
}
