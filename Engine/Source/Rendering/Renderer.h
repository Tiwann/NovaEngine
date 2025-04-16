#pragma once
#include "CullMode.h"
#include "Math/Matrix4.h"
#include "GraphicsApi.h"

namespace Nova
{
    class Color;
    class VertexArray;
    class VertexBuffer;
    class IndexBuffer;
    class Shader;
    class Camera;
    class Application;
    struct Vector3;
    struct Vector2;
    class Sprite;
    
    enum class DrawMode
    {
        Points,
        Lines,
        LineStrip,
        LineLoop,
        Triangles,
    };

    enum class DepthFunction
    {
        Always,
        Never,
        Less,
        LessOrEqual,
        Equal,
        NotEqual,
        Greater,
        GreaterOrEqual,
    };

    enum class BlendMode
    {
        Zero,
        One,
        SourceColor,
        OneMinusSourceColor,
        DestColor,
        OneMinusDestColor,
        SourceAlpha,
        OneMinusSourceAlpha,
        DestAlpha,
        OneMinusDestAlpha,
        ConstantColor,
        OnMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha,
        SourceAlphaSaturated,
        Source1Color,
        OneMinusSource1Color,
        Source1Alpha,
        OneMinusSource1Alpha,
    };

    enum class BlendOperation
    {
        Add,
        Min,
        Max,
        Subtract,
        ReverseSubtract,
    };



    class Renderer
    {
    public:
        Renderer(Application* Owner, const GraphicsApi& GraphicsApi) : m_Application(Owner), m_GraphicsApi(GraphicsApi) {}
        static Renderer* Create(Application* Owner, const GraphicsApi& GraphicsApi);

        bool ShouldRecreateSwapchain = false;

        virtual ~Renderer() = default;
        virtual bool Initialize() = 0;
        virtual void Destroy() = 0;
        virtual void ClearDepth(float Depth) = 0;
        virtual void ClearColor(const Color& Color) = 0;
        virtual void Clear(const Color& Color, float Depth);
        virtual void Present() = 0;
        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void SetViewportRect(Vector2 Position, Vector2 Size) = 0;
        virtual void Draw(DrawMode Mode, VertexArray* VAO, u32 NumVert, Shader* Shader) = 0;
        virtual void DrawIndexed(DrawMode Mode, VertexArray* VertexArray, VertexBuffer* VertexBuffer, IndexBuffer* IndexBuffer, Shader* Shader) = 0;
        virtual void DrawLine(const Vector3& PosA, const Vector3& PosB, f32 Thickness, const Color& Color) = 0;
        virtual void DrawWireQuad(const Matrix4& Transform, const Vector3& Position, const Vector2& HalfExtents, f32 Thickness, const Color& Color) = 0;
        virtual void DrawCircle(const Matrix4& Transform, const Vector3& Position, f32 Radius, const Color& Color) = 0;
        virtual void Blit() = 0;
        virtual void SetCullMode(CullMode Mode) = 0;
        virtual void SetDepthFunction(DepthFunction DepthFunction) = 0;
        virtual void SetBlendFunction(BlendMode ColorSource, BlendMode ColorDest, BlendOperation ColorOperation, BlendMode AlphaSource, BlendMode AlphaDest, BlendOperation AlphaOperation) = 0;
        virtual void SetBlendFunction(BlendMode Source, BlendMode Destination, BlendOperation Operation) = 0;
        virtual void SetBlending(bool Enabled) = 0;
        
        void SetCurrentCamera(Camera* Camera);
        Camera* GetCurrentCamera();
        const Camera* GetCurrentCamera() const;
        GraphicsApi GetGraphicsApi() const;
       
        static inline VertexBuffer* QuadVertexBuffer = nullptr;
        static inline VertexBuffer* CubeVertexBuffer = nullptr;
    protected:
        Camera* m_CurrentCamera = nullptr;
        Application* m_Application = nullptr;
        GraphicsApi m_GraphicsApi;
    };
}
