#include "OpenGLPipeline.h"
#include <glad/gl.h>


namespace Nova
{
    OpenGLPipeline::OpenGLPipeline(Renderer* Renderer, const PipelineSpecification& Specification): Pipeline(Renderer, Specification)
    {
        glCreateVertexArrays(1, &m_Handle);
    }

    OpenGLPipeline::~OpenGLPipeline()
    {
        glDeleteVertexArrays(1, &m_Handle);
    }

    u32 OpenGLPipeline::GetVertexArrayObject() const
    {
        return m_Handle;
    }
}
