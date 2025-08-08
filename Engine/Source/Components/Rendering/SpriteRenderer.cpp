#include "SpriteRenderer.h"
#include "Game/Entity.h"
#include "Game/Scene.h"
#include "Math/Matrix4.h"
#include "Rendering/ShaderModule.h"
#include "Rendering/Vulkan/ShaderModule.h"
#include "Runtime/Application.h"
#include "Runtime/ShaderCompiler.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Utils/BufferUtils.h"
#include "Runtime/Path.h"

#include <vulkan/vulkan.h>
#include <utility>
#include <cstdint>


namespace Nova
{
    struct SpriteVertex
    {
        Vector2 position;
        Vector2 texCoord;
    };

    struct Uniforms
    {
        Vector2 tiling;
        float padding[2];
        Vector4 spriteScale[2];
        Vector4 color;
    };

    SpriteRenderer::SpriteRenderer(Entity* owner) : Component(owner, "Sprite Renderer")
    {
    }

    void SpriteRenderer::OnInit()
    {
        Application* application = GetApplication();
        Vulkan::Device* device = application->GetDevice();
        Rendering::RenderPass* renderPass = application->GetRenderPass();

        const uint32_t indices[] = { 0, 2, 1, 0, 3, 2 };
        m_IndexBuffer = CreateIndexBuffer(*device, indices, sizeof(indices));

        m_VertexBuffer.Initialize({ device, Rendering::BufferUsage::VertexBuffer, 4 * sizeof(SpriteVertex) });
        m_UniformBuffer.Initialize({ device, Rendering::BufferUsage::UniformBuffer, sizeof(Uniforms) });
        m_StagingBuffer.Initialize({ device, Rendering::BufferUsage::StagingBuffer, 4 * sizeof(SpriteVertex) });
        m_Sampler.Initialize(Rendering::SamplerCreateInfo(device));

        const String shaderPath = Path::Combine(NOVA_ENGINE_ROOT_DIR, "Engine/Assets/Shaders/Sprite.slang");
        ShaderCompiler compiler;
        compiler.AddEntryPoint({ "vert", ShaderStageFlagBits::Vertex });
        compiler.AddEntryPoint({ "frag", ShaderStageFlagBits::Fragment });
        compiler.Initialize();
        compiler.Compile(shaderPath, "Sprite", ShaderTarget::SPIRV);

        const Array<uint32_t> vertSpirv = compiler.GetEntryPointBinary(ShaderStageFlagBits::Vertex);
        const Array<uint32_t> fragSpirv = compiler.GetEntryPointBinary(ShaderStageFlagBits::Fragment);
        if (vertSpirv.IsEmpty() || fragSpirv.IsEmpty()) return;

        Vulkan::ShaderModule vertShaderModule = Rendering::ShaderModule::Create<Vulkan::ShaderModule>(*device, ShaderStageFlagBits::Vertex, vertSpirv);
        Vulkan::ShaderModule fragShaderModule = Rendering::ShaderModule::Create<Vulkan::ShaderModule>(*device, ShaderStageFlagBits::Fragment, fragSpirv);

        m_BindingSetLayout.Initialize({device});
        m_BindingSetLayout.SetBinding(0, {"sampler", ShaderStageFlagBits::Fragment, ResourceBindingType::Sampler, 1});
        m_BindingSetLayout.SetBinding(1, {"texture", ShaderStageFlagBits::Fragment, ResourceBindingType::SampledTexture, 1});
        m_BindingSetLayout.SetBinding(2, {"uniforms", ShaderStageFlagBits::Vertex | ShaderStageFlagBits::Fragment, ResourceBindingType::UniformBuffer, 1});
        m_BindingSetLayout.Build();

        m_DescriptorSetLayout = m_BindingSetLayout.GetDescriptorSetLayout();

        VkPushConstantRange range;
        range.offset = 0;
        range.size = sizeof(Matrix4);
        range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &range;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;

        Vulkan::DescriptorPool* descriptorPool = device->GetDescriptorPool();
        m_DescriptorSet = descriptorPool->AllocateDescriptorSet(m_BindingSetLayout);

        VkResult result = vkCreatePipelineLayout(device->GetHandle(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout);
        if (result != VK_SUCCESS) return;

        Rendering::GraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.device = device;
        pipelineCreateInfo.renderPass = renderPass;
        pipelineCreateInfo.pipelineLayout = m_PipelineLayout;

        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "Position", Format::Vector2 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "TexCoords", Format::Vector2 });

        pipelineCreateInfo.inputAssemblyInfo.topology = PrimitiveTopology::TriangleList;
        pipelineCreateInfo.inputAssemblyInfo.primitiveRestartEnable = false;

        pipelineCreateInfo.rasterizationInfo.cullMode = CullMode::BackFace;
        pipelineCreateInfo.rasterizationInfo.frontFace = FrontFace::CounterClockwise;
        pipelineCreateInfo.rasterizationInfo.discardEnable = false;
        pipelineCreateInfo.rasterizationInfo.polygonMode = PolygonMode::Fill;
        pipelineCreateInfo.rasterizationInfo.depthClampEnable = false;
        pipelineCreateInfo.rasterizationInfo.depthBiasEnable = false;

        pipelineCreateInfo.colorBlendInfo.colorBlendEnable = false;

        pipelineCreateInfo.depthStencilInfo.depthTestEnable = false;
        pipelineCreateInfo.depthStencilInfo.depthWriteEnable = false;
        pipelineCreateInfo.depthStencilInfo.stencilTestEnable = false;
        pipelineCreateInfo.depthStencilInfo.depthCompareOp = CompareOperation::Less;

        pipelineCreateInfo.multisampleInfo.alphaToOneEnable = false;
        pipelineCreateInfo.multisampleInfo.sampleShadingEnable = false;
        pipelineCreateInfo.multisampleInfo.alphaToCoverageEnable = false;
        pipelineCreateInfo.multisampleInfo.sampleCount = 8;

        pipelineCreateInfo.viewportInfo.x = 0;
        pipelineCreateInfo.viewportInfo.y = 0;
        pipelineCreateInfo.viewportInfo.width = renderPass->GetWidth();
        pipelineCreateInfo.viewportInfo.height = renderPass->GetHeight();

        pipelineCreateInfo.scissorInfo.x = 0;
        pipelineCreateInfo.scissorInfo.y = 0;
        pipelineCreateInfo.scissorInfo.width = renderPass->GetWidth();
        pipelineCreateInfo.scissorInfo.height = renderPass->GetHeight();

        const VkPipelineShaderStageCreateInfo shaderStages[] { vertShaderModule.GetShaderStageCreateInfo(), fragShaderModule.GetShaderStageCreateInfo() };
        pipelineCreateInfo.shaderStages = shaderStages;
        pipelineCreateInfo.shaderStagesCount = std::size(shaderStages);
        m_Pipeline.Initialize(pipelineCreateInfo);
    }

    static void UpdateBuffer(Vulkan::Device& device, Vulkan::Buffer& buffer, const void* data, const size_t offset, const size_t size)
    {
        Vulkan::CommandPool* cmdPool = device.GetCommandPool();
        Vulkan::CommandBuffer cmdBuffer = cmdPool->AllocateCommandBuffer(Rendering::CommandBufferLevel::Primary);
        if (cmdBuffer.Begin({ Rendering::CommandBufferUsageFlagBits::OneTimeSubmit }))
        {
            vkCmdUpdateBuffer(cmdBuffer.GetHandle(), buffer.GetHandle(), offset, size, data);
            cmdBuffer.End();

            Vulkan::Fence fence;
            fence.Initialize({ &device });

            Vulkan::Queue* graphicsQueue = device.GetGraphicsQueue();
            graphicsQueue->Submit(&cmdBuffer, nullptr, nullptr, &fence);
            fence.Wait(~0);

            fence.Destroy();
            cmdBuffer.Free();
        }
    }

    void SpriteRenderer::OnUpdate(float deltaTime)
    {
        if(!m_Sprite.texture) return;
        if (m_Sprite.width == 0 || m_Sprite.height == 0) return;

        if (m_UpdateUniforms)
        {
            UpdateUniforms();
            m_UpdateUniforms = false;
        }

        if (m_UpdateResources)
        {
            UpdateResources();
            m_UpdateResources = false;
        }
    }

    void SpriteRenderer::OnRender(Rendering::CommandBuffer& cmdBuffer)
    {
        if(!m_Sprite.texture) return;
        Scene* scene = GetScene();

        Array<Camera*> cameras = scene->GetAllComponents<Camera>();
        if (cameras.IsEmpty()) return;

        Camera* camera = cameras.First();
        if (!camera->IsEnabled()) return;

        const Matrix4& viewProjection = camera->GetViewProjectionMatrix();
        const Matrix4& worldSpaceMatrix = GetTransform()->GetWorldSpaceMatrix();
        const Matrix4 mvp = viewProjection * worldSpaceMatrix;

        Application* application = GetApplication();
        const Rendering::RenderPass* renderPass = application->GetRenderPass();

        cmdBuffer.BindGraphicsPipeline(m_Pipeline);
        vkCmdBindDescriptorSets(((Vulkan::CommandBuffer&)cmdBuffer).GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
        cmdBuffer.BindVertexBuffer(m_VertexBuffer, 0);
        cmdBuffer.BindIndexBuffer(m_IndexBuffer, 0, Format::Uint32);
        cmdBuffer.SetViewport(renderPass->GetOffsetX(), renderPass->GetOffsetY(), renderPass->GetWidth(), renderPass->GetHeight(), 0.0f, 1.0f);
        cmdBuffer.SetScissor(renderPass->GetOffsetX(), renderPass->GetOffsetY(), renderPass->GetWidth(), renderPass->GetHeight());
        cmdBuffer.PushConstants(ShaderStageFlagBits::Vertex, 0, sizeof(Matrix4), &mvp, m_PipelineLayout);
        cmdBuffer.DrawIndexed(6, 0);
    }

    void SpriteRenderer::OnDestroy()
    {
        Component::OnDestroy();
    }

    Sprite& SpriteRenderer::GetSprite()
    {
        return m_Sprite;
    }

    void SpriteRenderer::SetSprite(const Sprite& sprite)
    {
        if (!sprite.texture) return;
        if (sprite == m_Sprite) return;

        m_Sprite = sprite;
        m_UpdateResources = true;
    }

    Vector2 SpriteRenderer::GetTiling() const
    {
        return m_Tiling;
    }

    void SpriteRenderer::SetTiling(const Vector2& tiling)
    {
        m_Tiling = tiling;
        m_UpdateUniforms = true;
    }

    uint32_t SpriteRenderer::GetPixelsPerUnit() const
    {
        return m_PixelsPerUnit;
    }

    void SpriteRenderer::SetPixelsPerUnit(uint32_t pixelsPerUnit)
    {
        m_PixelsPerUnit = pixelsPerUnit;
        m_UpdateUniforms = true;
    }

    Color SpriteRenderer::GetColorTint() const
    {
        return m_ColorTint;
    }

    void SpriteRenderer::SetColorTint(const Color& color)
    {
        m_ColorTint = color;
        m_UpdateUniforms = true;
    }

    SpriteRendererFlags SpriteRenderer::GetFlags() const
    {
        return m_Flags;
    }

    void SpriteRenderer::SetFlags(const SpriteRendererFlags flags)
    {
        m_Flags = flags;
        m_UpdateUniforms = true;
        m_UpdateResources = true;
    }

    void SpriteRenderer::UpdateUniforms()
    {
        const Vector2 tiling = m_Flags.Contains(SpriteRendererFlagBits::TileWithScale)
        ? (Vector2)GetTransform()->GetScale()
        : m_Tiling;

        const Matrix2 spriteScale = m_Sprite.texture
        ? Math::Scale(Matrix2::Identity, Vector2(m_Sprite.width, m_Sprite.height) / (float)m_PixelsPerUnit)
        : Matrix2::Identity;

        Uniforms uniforms;
        uniforms.tiling = tiling;
        uniforms.spriteScale[0] = Vector4(spriteScale[0].x, spriteScale[0].y, 0.0f, 0.0f);
        uniforms.spriteScale[1] = Vector4(spriteScale[1].x, spriteScale[1].y, 0.0f, 0.0f);
        uniforms.color = m_ColorTint;

        Application* application = GetApplication();
        Vulkan::Device* device = application->GetDevice();
        UpdateBuffer(*device, m_UniformBuffer, &uniforms, 0, sizeof(Uniforms));
    }

    void SpriteRenderer::UpdateResources()
    {
        const uint32_t textureWidth = m_Sprite.texture->GetWidth();
        const uint32_t textureHeight = m_Sprite.texture->GetHeight();
        if (textureWidth == 0 && textureHeight == 0) return;

        Vector2 uv0 = { m_Sprite.x / (float)textureWidth, (m_Sprite.y + m_Sprite.height) / (float)textureHeight };
        Vector2 uv1 = { (m_Sprite.x + m_Sprite.width) / (float)textureWidth, (m_Sprite.y + m_Sprite.height) / (float)textureHeight };
        Vector2 uv2 = { (m_Sprite.x + m_Sprite.width) / (float)textureWidth, m_Sprite.y / (float)textureHeight };
        Vector2 uv3 = { m_Sprite.x / (float)textureWidth, m_Sprite.y / (float)textureHeight };

        if(m_Flags.Contains(SpriteRendererFlagBits::FlipHorizontal))
        {
            std::swap(uv0, uv1);
            std::swap(uv2, uv3);
        }

        if(m_Flags.Contains(SpriteRendererFlagBits::FlipVertical))
        {
            std::swap(uv0, uv2);
            std::swap(uv1, uv3);
        }

        const SpriteVertex vertices[]
        {
            { {-0.5f, +0.5f }, uv0 },
            { {+0.5f, +0.5f }, uv1 },
            { {+0.5f, -0.5f }, uv2 },
            { {-0.5f, -0.5f }, uv3 },
        };

        m_StagingBuffer.CPUCopy(vertices, 0, sizeof(vertices));
        m_StagingBuffer.GPUCopy(m_VertexBuffer, 0, 0, sizeof(vertices));

        const VkDescriptorImageInfo imageInfo
        {
            m_Sampler.GetHandle(),
            ((Vulkan::Texture*)m_Sprite.texture)->GetImageView(),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        const VkDescriptorBufferInfo bufferInfo
        {
            .buffer = m_UniformBuffer.GetHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE
        };

        Array<VkWriteDescriptorSet> writes;

        VkWriteDescriptorSet samplerWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        samplerWrite.dstSet = m_DescriptorSet;
        samplerWrite.dstBinding = 0;
        samplerWrite.dstArrayElement = 0;
        samplerWrite.descriptorCount = 1;
        samplerWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        samplerWrite.pImageInfo = &imageInfo;
        writes.Add(samplerWrite);

        VkWriteDescriptorSet textureWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        textureWrite.dstSet = m_DescriptorSet;
        textureWrite.dstBinding = 1;
        textureWrite.dstArrayElement = 0;
        textureWrite.descriptorCount = 1;
        textureWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        textureWrite.pImageInfo = &imageInfo;
        writes.Add(textureWrite);

        VkWriteDescriptorSet bufferWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        bufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        bufferWrite.dstSet = m_DescriptorSet;
        bufferWrite.dstBinding = 2;
        bufferWrite.dstArrayElement = 0;
        bufferWrite.descriptorCount = 1;
        bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bufferWrite.pBufferInfo = &bufferInfo;
        writes.Add(bufferWrite);

        Application* application = GetApplication();
        const Vulkan::Device* device = application->GetDevice();
        vkUpdateDescriptorSets(device->GetHandle(), writes.Count(), writes.Data(), 0, nullptr);
    }
}
