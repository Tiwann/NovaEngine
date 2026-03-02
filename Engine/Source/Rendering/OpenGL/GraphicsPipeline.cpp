#include "GraphicsPipeline.h"
#include "Shader.h"
#include "Conversions.h"
#include "Format.h"
#include "Runtime/Log.h"
#include "RenderDevice.h"
#include <glad/glad.h>


namespace Nova::OpenGL
{
    bool GraphicsPipeline::Initialize(const GraphicsPipelineCreateInfo& createInfo)
    {
        if(!createInfo.device)
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to create graphics pipeline: device is null!");
            return false;
        }

        if(!createInfo.shader)
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to create graphics pipeline: shader is null!");
            return false;
        }

        if (HandleIsValid(m_VertexArrayObject))
            glDeleteVertexArrays(1, &m_VertexArrayObject);

        glCreateVertexArrays(1, &m_VertexArrayObject);

        const VertexLayout& vertexLayout = createInfo.vertexInputInfo.layout;
        const Array<VertexAttribute>& vertexAttributes = vertexLayout.GetAttributes();
        for (uint32_t i = 0; i < vertexAttributes.Count(); i++)
        {
            const VertexAttribute& vertexAttribute = vertexAttributes[i];

            glEnableVertexArrayAttrib(m_VertexArrayObject, i);
            glVertexArrayAttribBinding(m_VertexArrayObject, i, 0);
            glVertexArrayAttribFormat(m_VertexArrayObject, i,
                                      GetFormatComponentCount(vertexAttribute.format),
                                      Convert<GLformat>(vertexAttribute.format).type,
                                      GL_FALSE,
                                      vertexLayout.GetOffset(vertexAttribute));

        }

        m_PipelineDesc = createInfo;
        return true;
    }

    void GraphicsPipeline::Destroy()
    {
        if (HandleIsValid(m_VertexArrayObject))
            glDeleteVertexArrays(1, &m_VertexArrayObject);
    }

    void GraphicsPipeline::Bind() const
    {
        glBindVertexArray(m_VertexArrayObject);

        const auto glEnableState = [](GLenum state, bool value)
        {
            const auto enableFunc = value ? glEnable : glDisable;
            enableFunc(state);
        };

        // RASTERIZATION STATE
        const bool enableCullFace = m_PipelineDesc.rasterizationInfo.cullMode != CullMode::None;
        glEnableState(GL_CULL_FACE, enableCullFace);
        if (enableCullFace)
        {
            const GLenum cullMode = Convert<GLenum>(m_PipelineDesc.rasterizationInfo.cullMode);
            glCullFace(cullMode);
        }

        const GLenum frontFace = Convert<GLenum>(m_PipelineDesc.rasterizationInfo.frontFace);
        glFrontFace(frontFace);

        const GLenum polygonMode = Convert<GLenum>(m_PipelineDesc.rasterizationInfo.polygonMode);
        glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
        glEnableState(GL_RASTERIZER_DISCARD, m_PipelineDesc.rasterizationInfo.discardEnable);
        glEnableState(GL_DEPTH_CLAMP, m_PipelineDesc.rasterizationInfo.depthClampEnable);
        glLineWidth(m_PipelineDesc.rasterizationInfo.lineWidth);


        // COLOR BLEND
        const auto& colorBlendState = m_PipelineDesc.colorBlendInfo;
        const auto& blendFunc = colorBlendState.blendFunction;
        glEnableState(GL_BLEND, colorBlendState.colorBlendEnable);
        if (colorBlendState.colorBlendEnable)
        {
            glBlendFuncSeparate(Convert<GLenum>(blendFunc.colorSource), Convert<GLenum>(blendFunc.colorDest),
               Convert<GLenum>(blendFunc.alphaSource), Convert<GLenum>(blendFunc.alphaDest));
            glBlendEquationSeparate(Convert<GLenum>(blendFunc.colorOp), Convert<GLenum>(blendFunc.alphaOp));
        }
        const auto& colorWriteMask = colorBlendState.colorWriteMask;
        const bool writeRed = colorWriteMask.Contains(ColorChannelFlagBits::Red);
        const bool writeGreen = colorWriteMask.Contains(ColorChannelFlagBits::Green);
        const bool writeBlue = colorWriteMask.Contains(ColorChannelFlagBits::Blue);
        const bool writeAlpha = colorWriteMask.Contains(ColorChannelFlagBits::Alpha);
        glColorMask(writeRed, writeGreen, writeBlue, writeAlpha);

        // DEPTH STENCIL
        const auto& depthStencilState = m_PipelineDesc.depthStencilInfo;
        glEnableState(GL_DEPTH_TEST, depthStencilState.depthTestEnable);
        glEnableState(GL_STENCIL_TEST, depthStencilState.stencilTestEnable);
        glDepthMask(depthStencilState.depthWriteEnable);

        // MULTISAMPLE
        const auto& multisampleState = m_PipelineDesc.multisampleInfo;
        const bool enableMultisample = multisampleState.sampleCount > 1 && multisampleState.sampleCount % 2 == 0 && multisampleState.sampleCount <= 16;
        glEnableState(GL_MULTISAMPLE, enableMultisample);
        if (enableMultisample)
        {
            glEnableState(GL_SAMPLE_SHADING, multisampleState.sampleShadingEnable);
            glEnableState(GL_SAMPLE_ALPHA_TO_ONE, multisampleState.alphaToOneEnable);
            glEnableState(GL_SAMPLE_ALPHA_TO_COVERAGE, multisampleState.alphaToCoverageEnable);
        }

        //VIEWPORT
        const auto& viewportState = m_PipelineDesc.viewportInfo;
        glViewport(viewportState.x, viewportState.y, viewportState.width, viewportState.height);
        glDepthRange(viewportState.minDepth, viewportState.maxDepth);

        //SCISSOR
        const auto scissorState = m_PipelineDesc.scissorInfo;
        glScissor(scissorState.x, scissorState.y, scissorState.width, scissorState.height);

        const Shader* shader = static_cast<const Shader*>(m_PipelineDesc.shader);
        glUseProgram(shader->GetHandle());
    }
}
