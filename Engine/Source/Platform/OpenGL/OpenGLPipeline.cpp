#include "OpenGLPipeline.h"
#include <glad/gl.h>


namespace Nova
{
    OpenGLPipeline::OpenGLPipeline(Renderer* Renderer) : Pipeline(Renderer), m_Handle(U32_MAX)
    {

    }

    bool OpenGLPipeline::Initialize(const PipelineSpecification& Specification)
    {
        Pipeline::Initialize(Specification);
        glCreateVertexArrays(1, &m_Handle);
        return true;
    }

    void OpenGLPipeline::Destroy()
    {
        glDeleteVertexArrays(1, &m_Handle);
    }

    u32 OpenGLPipeline::GetVertexArrayObject() const
    {
        return m_Handle;
    }
}
