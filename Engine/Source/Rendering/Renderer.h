#pragma once
#include "Math/Matrix4.h"
#include "GraphicsApi.h"
#include "Containers/String.h"
#include "Runtime/Filesystem.h"
#include "CullMode.h"
#include "BlendOperation.h"
#include "BlendFactor.h"
#include "CompareOperation.h"
#include "Containers/Buffer.h"
#include "Vertex.h"
#include "Runtime/Object.h"

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
    class Pipeline;
    struct PipelineSpecification;
    class VertexBuffer;

    class Renderer : public Object
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
        virtual void Draw(VertexArray* VAO, u32 NumVert, Shader* Shader) = 0;
        virtual void DrawIndexed(VertexArray* VertexArray, VertexBuffer* VertexBuffer, IndexBuffer* IndexBuffer, Shader* Shader) = 0;
        virtual void SetCullMode(CullMode Mode) = 0;
        virtual void SetDepthCompareOperation(CompareOperation DepthFunction) = 0;
        virtual void SetBlendFunction(BlendFactor ColorSource, BlendFactor ColorDest, BlendOperation ColorOperation, BlendFactor AlphaSource, BlendFactor AlphaDest, BlendOperation AlphaOperation) = 0;
        virtual void SetBlendFunction(BlendFactor Source, BlendFactor Destination, BlendOperation Operation) = 0;
        virtual void SetBlending(bool Enabled) = 0;

        virtual void BindPipeline(Pipeline* Pipeline) = 0;


        Shader* CreateShader(const String& Name, const Path& Filepath);
        Pipeline* CreatePipeline(const PipelineSpecification& Specification);
        VertexBuffer* CreateVertexBuffer();
        VertexBuffer* CreateVertexBuffer(const BufferView<Vertex>& Vertices);
        IndexBuffer* CreateIndexBuffer();
        IndexBuffer* CreateIndexBuffer(const BufferView<u32>& Indices);

        void SetCurrentCamera(Camera* Camera);
        Camera* GetCurrentCamera();
        const Camera* GetCurrentCamera() const;
        GraphicsApi GetGraphicsApi() const;
        Application* GetOwner();
        const Application* GetOwner() const;

        template<typename RendererType> requires IsBaseOfValue<Renderer, RendererType>
        RendererType* As() { return dynamic_cast<RendererType*>(this); }
    protected:
        Camera* m_CurrentCamera = nullptr;
        Application* m_Application = nullptr;
        GraphicsApi m_GraphicsApi;
    };
}
