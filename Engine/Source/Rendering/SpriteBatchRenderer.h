#pragma once
#include "SpriteRendererFlags.h"
#include "Math/Quaternion.h"
#include "Math/Vector2.h"
#include "Runtime/Sprite.h"

namespace Nova
{
    class Camera;
    class Matrix4;
    struct Color;
    class CommandBuffer;
    class Device;
    class RenderPass;
    class Transform;
    class Shader;

    struct SpriteBatchRendererCreateInfo
    {
        RenderDevice* device = nullptr;
        RenderPass* renderPass = nullptr;
        Shader* shader = nullptr;
    };

    class SpriteBatchRenderer
    {
    public:
        static bool Initialize(const SpriteBatchRendererCreateInfo& createInfo);
        static void Destroy();

        static bool BeginFrame(const Matrix4& inViewProjection);
        static bool EndFrame(CommandBuffer& cmdBuffer);

        static void Render(CommandBuffer& cmdBuffer);

        static void DrawSprite(Sprite sprite, const Matrix4& transform, SpriteRendererFlags flags, const Color& colorTint, Vector2 tiling, Vector2 offset, float pixelsPerUnit);
    };
}
