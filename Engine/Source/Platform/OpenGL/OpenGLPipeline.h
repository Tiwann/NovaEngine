#pragma once
#include "Rendering/Pipeline.h"


namespace Nova
{
    class OpenGLPipeline : public Pipeline
    {
    public:
        OpenGLPipeline(Renderer* Renderer, const PipelineSpecification& Specification);
        virtual ~OpenGLPipeline();
        u32 GetVertexArrayObject() const;
    private:
        u32 m_Handle;
    };
}
