#include "ComputePipeline.h"
#include "RenderDevice.h"
#include "Shader.h"
#include "Runtime/Log.h"
#include <glad/glad.h>


namespace Nova::OpenGL
{
    bool ComputePipeline::Initialize(const ComputePipelineCreateInfo& createInfo)
    {
        if(!createInfo.device)
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to create compute pipeline: device is null!");
            return false;
        }

        if(!createInfo.shader)
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to create compute pipeline: shader is null!");
            return false;
        }

        m_PipelineDesc = createInfo;
        return true;
    }

    void ComputePipeline::Destroy()
    {
    }

    void ComputePipeline::Bind() const
    {
        const Shader* shader = static_cast<const Shader*>(m_PipelineDesc.shader);
        glUseProgram(shader->GetHandle());
    }
}
