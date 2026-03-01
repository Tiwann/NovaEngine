#pragma once
#include "Rendering/ComputePipeline.h"

namespace Nova::OpenGL
{
    class ComputePipeline : public Nova::ComputePipeline
    {
    public:
        bool Initialize(const ComputePipelineCreateInfo& createInfo);
        void Destroy();

    private:
        friend class Queue;
        void Bind() const;
        ComputePipelineCreateInfo m_PipelineDesc;
    };
}