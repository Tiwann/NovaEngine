#include "SpriteBatchRenderer.h"
#include "GraphicsPipeline.h"
#include "Buffer.h"
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
        int32_t frameIndex;
        int32_t cols;
        int32_t rows;
        int32_t flags;
        Vector2 tiling;
        Vector2 offset;
        Vector4 scale[2];
        Color color;
        uint32_t textureIndex;
    };

    static Matrix4 viewProjection;
    static Ref<GraphicsPipeline> graphicsPipeline = nullptr;
    static Ref<Buffer> uniformBuffer = nullptr;
    static Ref<ShaderBindingSet> bindingSet = nullptr;
    static uint32_t spriteCount = 0;
    static bool begin = false;
    static Array<Texture*> textures;
    static Array<SpriteData> uniforms;

    bool SpriteBatchRenderer::Initialize(const SpriteBatchRendererCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;

        const Application& application = Application::GetCurrentApplication();
        const AssetDatabase& assetDatabase = application.GetAssetDatabase();

        Ref<Shader> shader = assetDatabase.Get<Shader>("SpriteBatchShader");
        if (!shader) return false;

        GraphicsPipelineCreateInfo pipelineCreateInfo = GraphicsPipelineCreateInfo()
        .SetDevice(createInfo.device)
        .SetRenderPass(createInfo.renderPass)
        .SetShader(shader)
        .SetMultisampleInfo({8})
        .SetViewportInfo({0, 0, createInfo.renderPass->GetWidth(), createInfo.renderPass->GetHeight()})
        .SetScissorInfo({0, 0, createInfo.renderPass->GetWidth(), createInfo.renderPass->GetHeight()});

        graphicsPipeline = createInfo.device->CreateGraphicsPipeline(pipelineCreateInfo);
        if (!graphicsPipeline) return false;

        bindingSet = shader->CreateBindingSet(0);
        if (!bindingSet) return false;
        return true;
    }

    void SpriteBatchRenderer::Destroy()
    {
    }

    bool SpriteBatchRenderer::BeginFrame(const Matrix4& inViewProjection)
    {
        NOVA_ASSERT(!begin, "SpriteBatchRenderer::Begin/End call mismatch!");
        if (begin) return false;
        spriteCount = 0;
        viewProjection = inViewProjection;
        textures.Clear();
        uniforms.Clear();
        begin = true;
        return true;
    }

    bool SpriteBatchRenderer::EndFrame(CommandBuffer& cmdBuffer)
    {
        NOVA_ASSERT(begin, "SpriteBatchRenderer::End/End call mismatch!");
        if (!begin) return false;

        bindingSet->BindTexture()
        // Update buffers
        begin = false;
    }

    void SpriteBatchRenderer::Render(CommandBuffer& cmdBuffer)
    {
        if (spriteCount <= 0) return;
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

        SpriteData spriteData;
        spriteData.flags = flags.As<int32_t>();
        spriteData.frameIndex = 0;
        spriteData.rows = 0;
        spriteData.cols = 0;
        spriteData.tiling = finalTiling;
        spriteData.offset = offset;
        spriteData.scale[0] = Vector4(spriteScale[0].x, spriteScale[0].y, 0.0f, 0.0f);
        spriteData.scale[1] = Vector4(spriteScale[1].x, spriteScale[1].y, 0.0f, 0.0f);
        spriteData.color = colorTint;
        uniforms.Add(spriteData);
        spriteCount++;
    }
}
