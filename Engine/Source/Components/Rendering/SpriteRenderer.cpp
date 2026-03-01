#include "SpriteRenderer.h"
#include "Runtime/Entity.h"
#include "Runtime/Scene.h"
#include "Math/Matrix4.h"
#include "Rendering/ShaderModule.h"
#include "Runtime/Application.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Utils/BufferUtils.h"
#include "Rendering/RenderDevice.h"
#include "Rendering/Shader.h"
#include "Runtime/SpriteAnimation.h"
#include "Rendering/ShaderBindingSet.h"
#include "Rendering/Buffer.h"
#include "Rendering/CommandBuffer.h"

#include <imgui.h>

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
        Ref<RenderDevice> device = application->GetRenderDevice();
        const Ref<Window> window = application->GetWindow();
        const Rect2D<uint32_t> viewport = window->GetBounds();

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

        m_UniformBuffer = device->CreateBuffer({ device, BufferUsage::UniformBuffer, sizeof(Uniforms) });
        m_StagingBuffer = device->CreateBuffer({ device, BufferUsage::StagingBuffer, 4 * sizeof(SpriteVertex) });
        if (!m_Sampler) m_Sampler = device->GetOrCreateSampler(SamplerCreateInfo(device));

        const AssetDatabase& assetDatabase = application->GetAssetDatabase();
        m_Shader = assetDatabase.Get<Shader>("SpriteShader");
        m_BindingSet = m_Shader->CreateBindingSet(0);

        GraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.device = device;
        pipelineCreateInfo.shader = m_Shader;

        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "Position", Format::Vector2 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "TexCoords", Format::Vector2 });
        pipelineCreateInfo.multisampleInfo.sampleCount = 8;

        pipelineCreateInfo.viewportInfo.x = viewport.x;
        pipelineCreateInfo.viewportInfo.y = viewport.y;
        pipelineCreateInfo.viewportInfo.width = viewport.width;
        pipelineCreateInfo.viewportInfo.height = viewport.height;

        pipelineCreateInfo.scissorInfo.x =  viewport.x;
        pipelineCreateInfo.scissorInfo.y = viewport.y;
        pipelineCreateInfo.scissorInfo.width = viewport.width;
        pipelineCreateInfo.scissorInfo.height = viewport.height;
        m_Pipeline = device->CreateGraphicsPipeline(pipelineCreateInfo);

        m_BindingSet->BindSampler(0, *m_Sampler);
        m_BindingSet->BindBuffer(2, *m_UniformBuffer, 0, sizeof(Uniforms));
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

    void SpriteRenderer::OnPreRender(CommandBuffer& cmdBuffer)
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

    void SpriteRenderer::OnRender(CommandBuffer& cmdBuffer)
    {
        if(!m_Sprite.texture) return;

        Application* application = GetApplication();
        const auto window = application->GetWindow();
        const auto viewport = window->GetBounds();



        m_BindingSet->BindTexture(1, *m_Sprite.texture, BindingType::SampledTexture);
        cmdBuffer.BindGraphicsPipeline(*m_Pipeline);
        cmdBuffer.BindShaderBindingSet(*m_Shader, *m_BindingSet);
        cmdBuffer.BindVertexBuffer(*m_VertexBuffer, 0);
        cmdBuffer.BindIndexBuffer(*m_IndexBuffer, 0, Format::Uint32);
        cmdBuffer.SetViewport(viewport.x, viewport.y, viewport.width, viewport.height, 0.0f, 1.0f);
        cmdBuffer.SetScissor(viewport.x, viewport.y, viewport.width, viewport.height);
        cmdBuffer.DrawIndexed(6, 1, 0, 0, 0);
    }

    void SpriteRenderer::OnDestroy()
    {
        const Application* application = GetApplication();
        const Ref<RenderDevice>& device = application->GetRenderDevice();
        device->WaitIdle();

        m_BindingSet->Destroy();
        m_VertexBuffer->Destroy();
        m_IndexBuffer->Destroy();
        m_UniformBuffer->Destroy();
        m_StagingBuffer->Destroy();
        m_Sampler->Destroy();
        m_Pipeline->Destroy();
    }

    void SpriteRenderer::OnGui()
    {
        ImGui::DragFloat2("Tiling", m_Tiling.ValuePtr(), 0.1f);

        const char* flagNames[] = { "None",
                                    "Tile With Scale",
                                    "Flip Horizontal",
                                    "Flip Vertical"};


        if(ImGui::BeginCombo("Flags", nullptr, ImGuiComboFlags_NoPreview))
        {
            for(size_t i = 1; i < std::size(flagNames); i++)
            {
                bool isSlected = m_Flags.Contains(SpriteRendererFlagBits(1 << i));
                if(ImGui::Selectable(flagNames[i], &isSlected))
                {
                    m_Flags.Toggle(SpriteRendererFlagBits(1 << i));
                }
            }
            ImGui::EndCombo();
        }

        ImGui::ColorEdit4("Tint", (float*)&m_ColorTint, ImGuiColorEditFlags_DisplayHex);
        ImGui::DragInt("Pixels Per Unit", (int*)&m_PixelsPerUnit, 1);
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
        const Ref<RenderDevice>& device = application->GetRenderDevice();
        device->WaitIdle();

        m_Sprite = sprite;
        m_SpriteAnimation = nullptr;

        m_SpriteIndex = 0;
        m_Time = 0.0f;
        m_BindingSet->BindTexture(1, *m_Sprite.texture, BindingType::SampledTexture);
    }

    void SpriteRenderer::SetSprite(Ref<Texture> texture)
    {
        SetSprite({0, 0, texture->GetWidth(), texture->GetHeight(), texture});
    }

    void SpriteRenderer::SetSpriteAnimation(SpriteAnimation* spriteAnimation)
    {
        const Application* application = GetApplication();
        const Ref<RenderDevice>& device = application->GetRenderDevice();
        device->WaitIdle();

        m_SpriteAnimation = spriteAnimation;
        m_SpriteIndex = 0;
        m_Time = 0.0f;
        m_Sprite = spriteAnimation->GetSprite(m_SpriteIndex);
        m_BindingSet->BindTexture(1, *m_Sprite.texture, BindingType::SampledTexture);
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
