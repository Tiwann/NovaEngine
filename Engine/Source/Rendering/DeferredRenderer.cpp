#include "DeferredRenderer.h"
#include "Device.h"
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "Texture.h"

namespace Nova
{
    bool DeferredRenderer::Initialize(const DeferredRendererCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (createInfo.width == 0 || createInfo.height == 0) return false;
        if (createInfo.gBufferDescription.numPasses == 0) return false;
        if (createInfo.gBufferDescription.passesDescriptions == nullptr) return false;
        if (createInfo.renderTarget == nullptr) return false;

        if (m_IsValid)
            Invalidate();

        const BufferView<GBufferPassDescription> passesDescriptions{createInfo.gBufferDescription.passesDescriptions, createInfo.gBufferDescription.numPasses};

        m_RenderPass = new RenderPass();
        for (const GBufferPassDescription& passDesc : passesDescriptions)
        {
            RenderPassAttachment attachement;
            attachement.type = AttachmentType::Color;
            attachement.loadOp = LoadOperation::Clear;
            attachement.storeOp = StoreOperation::Store;
            attachement.clearValue.color = Color::Black;
            attachement.resolveMode = ResolveMode::Average;
            m_RenderPass->AddAttachment(attachement);
        }
        m_RenderPass->Initialize(0, 0, createInfo.width, createInfo.height);


        for (const GBufferPassDescription& passDesc : passesDescriptions)
        {
            if (!passDesc.shader)
                continue;

            GBufferPass pass;
            pass.description = passDesc;

            const VertexLayout vertexLayout({
                VertexAttribute{"POSITION", Format::Vector3},
                VertexAttribute{"TEXCOORDINATE", Format::Vector2},
                VertexAttribute{"NORMAL", Format::Vector3},
                VertexAttribute{"TANGENT", Format::Vector3},
                VertexAttribute{"COLOR", Format::Vector4}
            });

            const GraphicsPipelineCreateInfo pipelineCreateInfo = GraphicsPipelineCreateInfo()
                .SetDevice(createInfo.device)
                .SetShader(passDesc.shader)
                .SetRenderPass(m_RenderPass)
                .SetVertexLayout(VertexLayout())
                .SetMultisampleInfo({1})
                .SetDepthStencilInfo({
                    .depthTestEnable = true,
                    .depthWriteEnable = true,
                });

            pass.pipeline = createInfo.device->CreateGraphicsPipeline(pipelineCreateInfo);
            if (!pass.pipeline) return false;

            for (uint32_t i = 0; i < createInfo.device->GetImageCount(); i++)
            {
                TextureCreateInfo textureCreateInfo
                {
                    .device = createInfo.device,
                    .usageFlags = TextureUsageFlagBits::Sampled,
                    .format = pass.description.format,
                    .width = createInfo.width,
                    .height = createInfo.height,
                    .depth = 1,
                    .mips = 1,
                    .sampleCount = 1,
                    .data = nullptr,
                    .dataSize = 0
                };

                pass.textures[i] = createInfo.device->CreateTexture(textureCreateInfo);
                if (!pass.textures[i])
                {
                    pass.pipeline->Destroy();
                    return false;
                }
            }

            m_GBufferPasses.Add(pass);
        }


        m_Device = createInfo.device;
        m_RenderTarget = createInfo.renderTarget;
        m_Width = createInfo.width;
        m_Height = createInfo.height;
        m_GBufferDescription = createInfo.gBufferDescription;
        m_IsValid = true;
        return true;
    }

    void DeferredRenderer::Destroy()
    {
        Invalidate();
    }

    void DeferredRenderer::Resize(const uint32_t width, const uint32_t height)
    {
        const DeferredRendererCreateInfo createInfo
        {
            .device = m_Device,
            .renderTarget = m_RenderTarget,
            .width = width,
            .height = height,
            .gBufferDescription = m_GBufferDescription,
        };
        Initialize(createInfo);
    }

    bool DeferredRenderer::IsValid() const
    {
        return m_IsValid;
    }

    void DeferredRenderer::Invalidate()
    {
        for (GBufferPass& pass : m_GBufferPasses)
        {
            for (uint32_t i = 0; i < m_Device->GetImageCount(); i++)
                pass.textures[i]->Destroy();
            pass.pipeline->Destroy();
        }
        m_GBufferPasses.Clear();
        delete m_RenderPass;
        m_IsValid = false;
        m_Width = 0;
        m_Height = 0;
    }
}
