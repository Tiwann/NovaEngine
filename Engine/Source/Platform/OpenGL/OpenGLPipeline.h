#pragma once
#include "Rendering/Pipeline.h"


namespace Nova
{
    class OpenGLPipeline : public Pipeline
    {
    public:
        explicit OpenGLPipeline(Renderer* Renderer);
        ~OpenGLPipeline() override = default;

        bool Initialize(const PipelineCreateInfo& Specification) override;
        void Destroy() override;

        u32 GetVertexArrayObject() const;
    private:
        u32 m_Handle;
    };
}
