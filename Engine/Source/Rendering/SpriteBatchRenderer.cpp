#include "SpriteBatchRenderer.h"
#include "GraphicsPipeline.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include "Sampler.h"
#include "Shader.h"
#include "ShaderBindingSet.h"
#include "Components/Transform.h"
#include "Math/Matrix2.h"
#include "Math/Matrix4.h"
#include "Math/Vector2.h"
#include "Runtime/Application.h"

namespace Nova
{
    struct SpriteData
    {
        uint32_t id;
        int32_t frameIndex;
        int32_t cols;
        int32_t rows;
        int32_t flags;
        uint32_t padding0;
        Vector2 tiling;
        Vector2 offset;
        Matrix2 scale;
        Color color;
        Matrix4 worldToClip;
    };

    static Matrix4 viewProjection;
    static Ref<Device> device = nullptr;
    static Ref<GraphicsPipeline> graphicsPipeline = nullptr;
    static Ref<Buffer> storageBuffer = nullptr;
    static Ref<ShaderBindingSet> bindingSet = nullptr;
    static Ref<Sampler> sampler = nullptr;
    static RenderPass* renderPass = nullptr;
    static Shader* shader = nullptr;
    static bool begin = false;
    static Array<Texture*> textures;
    static Array<SpriteData> spriteData;

    bool SpriteBatchRenderer::Initialize(const SpriteBatchRendererCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (!createInfo.renderPass) return false;
        if (!createInfo.shader) return false;

        const GraphicsPipelineCreateInfo pipelineCreateInfo = GraphicsPipelineCreateInfo()
        .SetDevice(createInfo.device)
        .SetRenderPass(createInfo.renderPass)
        .SetShader(createInfo.shader)
        .SetMultisampleInfo({8})
        .SetViewportInfo({0, 0, createInfo.renderPass->GetWidth(), createInfo.renderPass->GetHeight()})
        .SetScissorInfo({0, 0, createInfo.renderPass->GetWidth(), createInfo.renderPass->GetHeight()});

        graphicsPipeline = createInfo.device->CreateGraphicsPipeline(pipelineCreateInfo);
        if (!graphicsPipeline) return false;

        bindingSet = createInfo.shader->CreateBindingSet(0);
        if (!bindingSet) return false;

        sampler = createInfo.device->GetOrCreateSampler(SamplerCreateInfo());
        device = createInfo.device;
        renderPass = createInfo.renderPass;
        shader = createInfo.shader;
        return true;
    }

    void SpriteBatchRenderer::Destroy()
    {
        if (bindingSet) bindingSet->Destroy();
        if (graphicsPipeline) graphicsPipeline->Destroy();
        if (storageBuffer) storageBuffer->Destroy();
        renderPass = nullptr;
        device = nullptr;
        shader = nullptr;
    }

    bool SpriteBatchRenderer::BeginFrame(const Matrix4& inViewProjection)
    {
        NOVA_ASSERT(!begin, "SpriteBatchRenderer::Begin/End call mismatch!");
        viewProjection = inViewProjection;
        textures.Clear();
        spriteData.Clear();
        begin = true;
        return true;
    }

    bool SpriteBatchRenderer::EndFrame(CommandBuffer& cmdBuffer)
    {
        NOVA_ASSERT(begin, "SpriteBatchRenderer::End/End call mismatch!");
        if (!begin) return false;

        if (!bindingSet->BindCombinedSamplerTextures(1, *sampler, textures.Data(), textures.Count()))
        {
            begin = false;
            return false;
        }

        if (!storageBuffer || storageBuffer->GetSize() < spriteData.Size())
        {
            Ref<Buffer> buffer = device->CreateBuffer(BufferUsage::StorageBuffer, spriteData.Size());
            if (!buffer)
            {
                begin = false;
                return false;
            }

            if (storageBuffer)
                storageBuffer->Destroy();
            storageBuffer = buffer;
            bindingSet->BindBuffer(2, *storageBuffer, 0, spriteData.Size());
        }

        cmdBuffer.UpdateBuffer(*storageBuffer, 0, spriteData.Size(), spriteData.Data());
        begin = false;
        return true;
    }

    void SpriteBatchRenderer::Render(CommandBuffer& cmdBuffer)
    {
        if (spriteData.IsEmpty()) return;

        cmdBuffer.BindGraphicsPipeline(*graphicsPipeline);
        cmdBuffer.BindShaderBindingSet(*shader, *bindingSet);
        cmdBuffer.SetViewport(renderPass->GetOffsetX(), renderPass->GetOffsetY(), renderPass->GetWidth(), renderPass->GetHeight(), 0.0f, 1.0f);
        cmdBuffer.SetScissor(renderPass->GetOffsetX(), renderPass->GetOffsetY(), renderPass->GetWidth(), renderPass->GetHeight());
        cmdBuffer.Draw(6, spriteData.Count(), 0, 0);
    }

    void SpriteBatchRenderer::DrawSprite(Sprite sprite, const Matrix4& transform, const SpriteRendererFlags flags, const Color& colorTint, Vector2 tiling, Vector2 offset, float pixelsPerUnit)
    {
        if (!sprite.texture) return;
        textures.AddUnique(sprite.texture);

        const Matrix4 mvp = viewProjection * transform;
        Vector3 scale;
        transform.Decompose(&scale, nullptr, nullptr);

        const Vector2 finalTiling = flags.Contains(SpriteRendererFlagBits::TileWithScale)
        ? static_cast<Vector2>(scale)
        : tiling;

        const Matrix2 spriteScale = Math::Scale(Matrix2::Identity, Vector2(sprite.width, sprite.height) / pixelsPerUnit);

        SpriteData data;
        data.id = textures.Find(sprite.texture);
        data.frameIndex = 0;
        data.rows = 0;
        data.cols = 0;
        data.flags = flags.As<int32_t>();
        data.tiling = finalTiling;
        data.offset = offset;
        data.scale = spriteScale;
        data.color = colorTint;
        data.worldToClip = mvp;
        spriteData.Add(data);
    }
}
