#pragma once
#include "Rendering/ComputePipeline.h"

namespace Nova::OpenGL
{
    class ComputePipeline final : public Nova::ComputePipeline
    {
    public:
        bool Initialize(const ComputePipelineCreateInfo& createInfo) override;
        void Destroy() override;

    private:
        friend class Queue;
        void Bind() const;
        ComputePipelineCreateInfo m_PipelineDesc;
    };
}