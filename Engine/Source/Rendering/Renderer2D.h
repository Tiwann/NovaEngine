#pragma once
#include "Math/Rect.h"
#include "Runtime/Ref.h"

namespace Nova
{
    class RenderDevice;
    class Shader;
    class GraphicsPipeline;
    class Buffer;
    struct Vector2;
    struct Color;
    class CommandBuffer;

    class Renderer2D : public RefCounted
    {
    public:
        Renderer2D() = default;
        ~Renderer2D() override = default;

        bool Initialize(Ref<RenderDevice> renderDevice);
        void Destroy();

        void Begin();
        void End();
        void Render(CommandBuffer& cmdBuffer);

        /// Draw a colored quad
        /// @param position Position in screen space
        /// @param size size of the quad in screen space
        /// @param rotation Rotation of the quad in radians
        /// @param color Color to fill the quad with
        void DrawQuad(const Vector2& position, const Vector2& size, float rotation, const Color& color);

        /// Draw a colored quad
        /// @param rect Rectangle to draw (position + size)
        /// @param rotation Rotation of the quad to draw in radians
        /// @param color Color to fill the quad with
        void DrawQuad(const Rect2D<float>& rect, float rotation, const Color& color);

        /// Draw a colored ellipse
        /// @param position Position of the top left corner of the circumscribed rectangle of the ellipse
        /// @param size Size of the ellipse
        /// @param rotation Rotation in radians of the ellipse
        /// @param color Color to fill the ellipse with
        void DrawEllipse(const Vector2& position, const Vector2& size, float rotation, const Color& color);

        /// Draw a colored ellipse
        /// @param rect Circumscribed rectangle of the ellipse
        /// @param rotation Rotation in radians of the ellipse
        /// @param color Color to fill the ellipse with
        void DrawEllipse(const Rect2D<float>& rect, float rotation, const Color& color);

        ///
        /// @param position Position of the top left corner of the circumscribed square of the circle
        /// @param radius Radius of the circle
        /// @param color Color to fill the circle with
        void DrawCircle(const Vector2& position, float radius, const Color& color);
    private:
        Ref<RenderDevice> m_RenderDevice = nullptr;
        Ref<Shader> m_Shader = nullptr;
        Ref<GraphicsPipeline> m_Pipeline = nullptr;
        Ref<Buffer> m_VertexBuffer = nullptr;
        Ref<Buffer> m_IndexBuffer = nullptr;
    };

}
