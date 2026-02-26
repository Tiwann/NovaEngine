#pragma once
#include "Runtime/Object.h"
#include "Runtime/Ref.h"


namespace Nova
{
    struct Vector3;
    struct Vector2;
    class Matrix4;
    struct Color;
    class CommandBuffer;
    class RenderDevice;
    class Shader;
    class RenderPass;
}

namespace Nova
{
    struct DebugRendererCreateInfo
    {
        Ref<RenderDevice> device = nullptr;
        Ref<Shader> shader = nullptr;
        RenderPass* renderPass = nullptr;
        uint32_t maxVertices = 0;
    };

    class DebugRenderer final : public Object
    {
    public:
        DebugRenderer() : Object("Debug Renderer") {}
        ~DebugRenderer() override = default;

        static bool Initialize(const DebugRendererCreateInfo& createInfo);
        static void Destroy();

        static void Begin(const Matrix4& viewProjection);
        static void End(CommandBuffer& cmdBuffer);
        static void Render(CommandBuffer& cmdBuffer);
        static void DrawLine(const Vector3& start, const Vector3& end, const Color& color, float thickness = 1.0f);
        static void DrawSquare(const Vector3& position, const Quaternion& rotation, const Vector3& size, const Color& color, float thickness = 1.0f);
        static void DrawCircle(const Vector3& position, const Quaternion& rotation, float radius, const Color& color, float thickness = 1.0f);
        static void DrawCapsule(const Vector3& position, float height, float radius, const Color& color, float thickness = 1.0f);
    private:
        static inline Ref<RenderDevice> s_Device = nullptr;
    };
}
