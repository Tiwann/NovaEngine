#include "SpriteRenderer.h"
#include "Runtime/Entity.h"
#include "Runtime/Scene.h"
#include "Math/Matrix4.h"
#include "Rendering/ShaderModule.h"
#include "Runtime/Application.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Utils/BufferUtils.h"
#include "Rendering/Vulkan/Device.h"
#include "Rendering/Shader.h"
#include "Runtime/SpriteAnimation.h"
#include "Rendering/ShaderBindingSet.h"

#include <vulkan/vulkan.h>
#include <cstdint>
#include <vma/vk_mem_alloc.h>

#include "Rendering/Vulkan/Buffer.h"

namespace Nova
{
    struct SpriteVertex
    {
        Vector2 position;
        Vector2 texCoord;
    };

    struct Uniforms
    {
        int32_t frameIndex;
        int32_t cols;
        int32_t rows;
        int32_t flags;
        Matrix4 mvp;
        Vector2 tiling;
        Vector2 offset;
        Vector4 scale[2];
        Vector4 color;
    };


    SpriteRenderer::SpriteRenderer(Entity* owner) : Component(owner, "Sprite Renderer")
    {

    }

    void SpriteRenderer::OnInit()
    {
        Application* application = GetApplication();
        Ref<Rendering::Device> device = application->GetDevice();
        Rendering::RenderPass* renderPass = application->GetRenderPass();

        const uint32_t indices[] = { 0, 2, 1, 0, 3, 2 };
        m_IndexBuffer = CreateIndexBuffer(device, indices, sizeof(indices));

        const SpriteVertex vertices[]
        {
            { {-0.5f, +0.5f }, {0.0f, 1.0f} },
            { {+0.5f, +0.5f }, {1.0f, 1.0f} },
            { {+0.5f, -0.5f }, {1.0f, 0.0f} },
            { {-0.5f, -0.5f }, {0.0f, 0.0f} },
        };
        m_VertexBuffer = CreateVertexBuffer(device, vertices, sizeof(vertices));

        m_UniformBuffer = device->CreateBuffer({ device, Rendering::BufferUsage::UniformBuffer, sizeof(Uniforms) });
        m_StagingBuffer = device->CreateBuffer({ device, Rendering::BufferUsage::StagingBuffer, 4 * sizeof(SpriteVertex) });
        m_Sampler = device->CreateSampler(Rendering::SamplerCreateInfo(device));

        const AssetDatabase& assetDatabase = application->GetAssetDatabase();
        m_Shader = assetDatabase.Get<Rendering::Shader>("SpriteShader");
        m_BindingSet = m_Shader->CreateBindingSet(0);

        Rendering::GraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.device = device;
        pipelineCreateInfo.renderPass = renderPass;
        pipelineCreateInfo.shader = m_Shader;

        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "Position", Format::Vector2 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "TexCoords", Format::Vector2 });
        pipelineCreateInfo.multisampleInfo.sampleCount = 8;

        pipelineCreateInfo.viewportInfo.x = 0;
        pipelineCreateInfo.viewportInfo.y = 0;
        pipelineCreateInfo.viewportInfo.width = renderPass->GetWidth();
        pipelineCreateInfo.viewportInfo.height = renderPass->GetHeight();

        pipelineCreateInfo.scissorInfo.x = 0;
        pipelineCreateInfo.scissorInfo.y = 0;
        pipelineCreateInfo.scissorInfo.width = renderPass->GetWidth();
        pipelineCreateInfo.scissorInfo.height = renderPass->GetHeight();
        m_Pipeline = device->CreateGraphicsPipeline(pipelineCreateInfo);

        m_BindingSet->BindSampler(0, m_Sampler);
        m_BindingSet->BindBuffer(2, m_UniformBuffer, 0, sizeof(Uniforms));
    }

    void SpriteRenderer::OnUpdate(const float deltaTime)
    {
        if (m_SpriteAnimation)
        {
            m_Time += deltaTime;
            if (m_Time >= m_Speed)
            {
                m_Time = 0.0f;
                m_SpriteIndex = (m_SpriteIndex + 1) % m_SpriteAnimation->Count();
                m_Sprite = m_SpriteAnimation->GetSprite(m_SpriteIndex);
            }
        } else
        {
            m_Time = 0.0f;
            m_SpriteIndex = 0;
        }
    }

    void SpriteRenderer::OnPreRender(Rendering::CommandBuffer& cmdBuffer)
    {
        Scene* scene = GetScene();

        Array<Camera*> cameras = scene->GetAllComponents<Camera>();
        if (cameras.IsEmpty()) return;

        Camera* camera = cameras.First();
        if (!camera->IsEnabled()) return;

        const Matrix4& viewProjection = camera->GetViewProjectionMatrix();
        const Matrix4& worldSpaceMatrix = GetTransform()->GetWorldSpaceMatrix();
        const Matrix4 mvp = viewProjection * worldSpaceMatrix;

        const Vector2 tiling = m_Flags.Contains(SpriteRendererFlagBits::TileWithScale)
        ? (Vector2)GetTransform()->GetScale()
        : m_Tiling;

        const Matrix2 spriteScale = m_Sprite.texture
        ? Math::Scale(Matrix2::Identity, Vector2(m_Sprite.width, m_Sprite.height) / (float)m_PixelsPerUnit)
        : Matrix2::Identity;

        Uniforms uniforms;
        uniforms.flags = m_Flags.As<int32_t>();
        uniforms.frameIndex = m_SpriteIndex;
        uniforms.rows = m_SpriteAnimation ? m_SpriteAnimation->GetSpriteSheet().rows : 0;
        uniforms.cols = m_SpriteAnimation ? m_SpriteAnimation->GetSpriteSheet().columns : 0;
        uniforms.mvp = mvp;
        uniforms.tiling = tiling;
        uniforms.offset = m_TilingOffset;
        uniforms.scale[0] = Vector4(spriteScale[0].x, spriteScale[0].y, 0.0f, 0.0f);
        uniforms.scale[1] = Vector4(spriteScale[1].x, spriteScale[1].y, 0.0f, 0.0f);
        uniforms.color = m_ColorTint;

        cmdBuffer.UpdateBuffer(*m_UniformBuffer, 0, sizeof(Uniforms), &uniforms);
    }

    void SpriteRenderer::OnRender(Rendering::CommandBuffer& cmdBuffer)
    {
        if(!m_Sprite.texture) return;

        Application* application = GetApplication();
        const Rendering::RenderPass* renderPass = application->GetRenderPass();


        m_BindingSet->BindTexture(1, m_Sprite.texture);
        cmdBuffer.BindGraphicsPipeline(*m_Pipeline);
        cmdBuffer.BindShaderBindingSet(*m_Shader, *m_BindingSet);
        cmdBuffer.BindVertexBuffer(*m_VertexBuffer, 0);
        cmdBuffer.BindIndexBuffer(*m_IndexBuffer, 0, Format::Uint32);
        cmdBuffer.SetViewport(renderPass->GetOffsetX(), renderPass->GetOffsetY(), renderPass->GetWidth(), renderPass->GetHeight(), 0.0f, 1.0f);
        cmdBuffer.SetScissor(renderPass->GetOffsetX(), renderPass->GetOffsetY(), renderPass->GetWidth(), renderPass->GetHeight());
        cmdBuffer.DrawIndexed(6, 0);
    }

    void SpriteRenderer::OnDestroy()
    {
        const Application* application = GetApplication();
        const Ref<Rendering::Device>& device = application->GetDevice();
        device->WaitIdle();

        m_BindingSet->Destroy();
        m_VertexBuffer->Destroy();
        m_IndexBuffer->Destroy();
        m_UniformBuffer->Destroy();
        m_StagingBuffer->Destroy();
        m_Sampler->Destroy();
        m_Pipeline->Destroy();
    }

    Sprite& SpriteRenderer::GetSprite()
    {
        return m_Sprite;
    }

    void SpriteRenderer::SetSprite(const Sprite& sprite)
    {
        if (!sprite.texture) return;
        if (sprite == m_Sprite) return;

        const Application* application = GetApplication();
        const Ref<Rendering::Device>& device = application->GetDevice();
        device->WaitIdle();

        m_Sprite = sprite;
        m_SpriteAnimation = nullptr;

        m_SpriteIndex = 0;
        m_Time = 0.0f;
        m_BindingSet->BindTexture(1, m_Sprite.texture);
    }

    void SpriteRenderer::SetSpriteAnimation(SpriteAnimation* spriteAnimation)
    {
        const Application* application = GetApplication();
        const Ref<Rendering::Device>& device = application->GetDevice();
        device->WaitIdle();

        m_SpriteAnimation = spriteAnimation;
        m_SpriteIndex = 0;
        m_Time = 0.0f;
        m_Sprite = spriteAnimation->GetSprite(m_SpriteIndex);
        m_BindingSet->BindTexture(1, m_Sprite.texture);
    }

    SpriteAnimation* SpriteRenderer::GetSpriteAnimation() const
    {
        return m_SpriteAnimation;
    }

    Vector2 SpriteRenderer::GetTiling() const
    {
        return m_Tiling;
    }

    void SpriteRenderer::SetTiling(const Vector2& tiling)
    {
        m_Tiling = tiling;
    }

    Vector2 SpriteRenderer::GetTilingOffset() const
    {
        return m_TilingOffset;
    }

    void SpriteRenderer::SetTilingOffset(const Vector2& tilingOffset)
    {
        m_TilingOffset = tilingOffset;
    }

    uint32_t SpriteRenderer::GetPixelsPerUnit() const
    {
        return m_PixelsPerUnit;
    }

    void SpriteRenderer::SetPixelsPerUnit(uint32_t pixelsPerUnit)
    {
        m_PixelsPerUnit = pixelsPerUnit;
    }

    Color SpriteRenderer::GetColorTint() const
    {
        return m_ColorTint;
    }

    void SpriteRenderer::SetColorTint(const Color& color)
    {
        m_ColorTint = color;
    }

    SpriteRendererFlags SpriteRenderer::GetFlags() const
    {
        return m_Flags;
    }

    void SpriteRenderer::SetFlags(const SpriteRendererFlags flags)
    {
        m_Flags = flags;
    }

    void SpriteRenderer::SetSpeed(int32_t fps)
    {
        m_Speed = 1.0f / fps;
    }

    void SpriteRenderer::SetSpeed(float speed)
    {
        m_Speed = speed;
    }

    float SpriteRenderer::GetSpeed() const
    {
        return m_Speed;
    }

}
