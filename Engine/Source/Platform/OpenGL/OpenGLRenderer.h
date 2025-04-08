#pragma once
#include "Graphics/CullMode.h"
#include "Graphics/Renderer.h"
#include "Runtime/LogCategory.h"
#include "Runtime/LogVerbosity.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(OpenGL, "OPENGL");

typedef Nova::u32 GLenum;

namespace Nova
{
    class Shader;
    
    class OpenGLRenderer : public Renderer
    {
    public:
        OpenGLRenderer(Application* Owner) : Renderer(Owner) {}
        ~OpenGLRenderer() override = default;

        bool Initialize() override;
        void Destroy() override;
        void ClearDepth(float Depth) override;
        void ClearColor(const Color& color) override;
        void Present() override;
        bool BeginFrame() override;
        void EndFrame() override;
        void SetViewportRect(Vector2 Position, Vector2 Size) override;
        void Draw(DrawMode Mode, VertexArray* VAO, u32 NumVert, Shader* Shader) override;
        void DrawIndexed(DrawMode Mode, VertexArray* VertexArray, VertexBuffer* VertexBuffer, IndexBuffer* IndexBuffer, Shader* Shader) override;
        void DrawLine(const Vector3& PointA, const Vector3& PointB, f32 Thickness, const Color& Color) override;
        void DrawWireQuad(const Matrix4& Transform, const Vector3& Position, const Vector2& HalfExtents, f32 Thickness, const Color& Color) override;
        void DrawCircle(const Matrix4& Transform, const Vector3& Position, f32 Radius, const Color& Color) override;
        void SetCullMode(CullMode Mode) override;
        void SetDepthFunction(DepthFunction DepthFunction) override;
        void SetBlendFunction(BlendMode Source, BlendMode Destination, BlendOperation Operation) override;
        void SetBlendFunction(BlendMode ColorSource, BlendMode ColorDest, BlendOperation ColorOperation, BlendMode AlphaSource, BlendMode AlphaDest, BlendOperation AlphaOperation) override; 
        void SetBlending(bool Enabled) override;
        void Blit() override;
        GLenum GetOpenGLDrawMode(DrawMode Mode);
        GLenum GetOpenGLBlendMode(BlendMode Mode);
        GLenum GetOpenGLBlendOperation(BlendOperation Operation);
    protected:
        static String GetDebugSourceName(u32 Source);
        static Verbosity GetDebugVerbosity(u32 Severity);
    };
}
