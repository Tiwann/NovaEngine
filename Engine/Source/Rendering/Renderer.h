#pragma once
#include "GraphicsApi.h"
#include "Containers/Buffer.h"
#include "Containers/String.h"
#include "Runtime/Filesystem.h"
#include "Runtime/Object.h"
#include "CullMode.h"
#include "BlendOperation.h"
#include "BlendFactor.h"
#include "CompareOperation.h"
#include "Scissor.h"
#include "Vertex.h"

namespace Nova
{
    class RenderTarget;
    struct RenderTargetCreateInfo;
    class UniformBuffer;
    class Color;
    class VertexArray;
    class VertexBuffer;
    struct VertexBufferCreateInfo;
    class IndexBuffer;
    struct IndexBufferCreateInfo;
    class Shader;
    class Camera;
    class Application;
    struct Vector3;
    struct Vector2;
    class Sprite;
    class Pipeline;
    struct PipelineCreateInfo;
    class VertexBuffer;
    class Swapchain;
    struct SwapchainCreateInfo;
    struct Viewport;

    class CommandPool;
    struct CommandPoolCreateInfo;
    class CommandBuffer;

    class Semaphore;
    struct SemaphoreCreateInfo;
    class Fence;
    struct FenceCreateInfo;

    class Surface;
    struct SurfaceCreateInfo;

    class Window;

#if defined(NOVA_DEV) || defined(NOVA_DEBUG)
    static constexpr bool RendererIsDebug = true;
#else
    static constexpr bool RendererIsDebug = false;
#endif

    struct RendererCreateInfo
    {
        Window* Window = nullptr;
    };

    class Renderer : public Object
    {
    public:
        Renderer(Application* Owner, const GraphicsApi& GraphicsApi) : m_Application(Owner), m_GraphicsApi(GraphicsApi) {}
        static Renderer* Create(Application* Owner, const GraphicsApi& GraphicsApi);

        bool ShouldRecreateSwapchain = false;

        ~Renderer() override = default;
        virtual bool Initialize(const RendererCreateInfo& CreateInfo) = 0;
        virtual void Destroy() = 0;
        virtual void ClearDepth(float Depth) = 0;
        virtual void ClearColor(const Color& Color) = 0;
        virtual void Clear(const Color& Color, float Depth);
        virtual void Present() = 0;
        virtual bool BeginFrame() = 0;
        virtual void BeginRendering() = 0;
        virtual void EndRendering() = 0;
        virtual void EndFrame() = 0;
        virtual void SetViewport(const Viewport& Viewport) = 0;
        virtual void SetScissor(const Scissor& Scissor) = 0;
        virtual void Draw(size_t VertexCount, size_t FirstVertex = 0) = 0;
        virtual void DrawIndexed(size_t IndexCount, size_t Offset) = 0;
        virtual void SetCullMode(CullMode Mode) = 0;
        virtual void SetDepthCompareOperation(CompareOperation DepthFunction) = 0;
        virtual void SetBlendFunction(BlendFactor ColorSource, BlendFactor ColorDest, BlendOperation ColorOperation, BlendFactor AlphaSource, BlendFactor AlphaDest, BlendOperation AlphaOperation) = 0;
        virtual void SetBlendFunction(BlendFactor Source, BlendFactor Destination, BlendOperation Operation) = 0;
        virtual void SetBlending(bool Enabled) = 0;

        virtual void BindPipeline(Pipeline* Pipeline) = 0;

        virtual void UpdateUniformBuffer(UniformBuffer* Buffer, u64 Offset, u64 Size, const void* Data) = 0;
        virtual void BindVertexBuffer(VertexBuffer* Buffer, u64 Offset) = 0;
        virtual void BindIndexBuffer(IndexBuffer* Buffer, u64 Offset) = 0;
        virtual void WaitIdle() const {}

        Surface* CreateSurface(const SurfaceCreateInfo& CreateInfo);
        template<typename SurfaceType> requires IsBaseOf<Surface, SurfaceType>::Value
        SurfaceType* CreateSurface(const SurfaceCreateInfo& CreateInfo)
        {
            return (SurfaceType*)CreateSurface(CreateInfo);
        }

        Semaphore* CreateSemaphore(const SemaphoreCreateInfo& CreateInfo);
        template<typename SemaphoreType> requires IsBaseOf<Semaphore, SemaphoreType>::Value
        SemaphoreType* CreateSurface(const SemaphoreCreateInfo& CreateInfo)
        {
            return (SemaphoreType*)CreateSemaphore(CreateInfo);
        }

        Fence* CreateFence(const FenceCreateInfo& CreateInfo);
        template<typename FenceType> requires IsBaseOf<Fence, FenceType>::Value
        FenceType* CreateFence(const FenceCreateInfo& CreateInfo)
        {
            return (FenceType*)CreateFence(CreateInfo);
        }

        RenderTarget* CreateRenderTarget(const RenderTargetCreateInfo& CreateInfo);
        template<typename RenderTargetType> requires IsBaseOf<RenderTarget, RenderTargetType>::Value
        RenderTargetType* CreateRenderTarget(const RenderTargetCreateInfo& CreateInfo)
        {
            return (RenderTargetType*)CreateRenderTarget(CreateInfo);
        }

        CommandPool* CreateCommandPool(const CommandPoolCreateInfo& CreateInfo);
        template<typename CommandPoolType> requires IsBaseOf<CommandPool, CommandPoolType>::Value
        CommandPoolType* CreateCommandPool(const CommandPoolCreateInfo& CreateInfo)
        {
            return (CommandPoolType*)CreateCommandPool(CreateInfo);
        }

        Swapchain* CreateSwapchain(const SwapchainCreateInfo& CreateInfo);
        template<typename SwapchainType> requires IsBaseOf<Swapchain, SwapchainType>::Value
        SwapchainType* CreateSwapchain(const SwapchainCreateInfo& CreateInfo)
        {
            return (SwapchainType*)CreateSwapchain(CreateInfo);
        }

        Shader* CreateShader(const String& Name, const Path& Filepath);
        template<typename ShaderType> requires IsBaseOf<Shader, ShaderType>::Value
        ShaderType* CreateShader(const String& Name, const Path& Filepath)
        {
            return (ShaderType*)CreateShader(Name, Filepath);
        }

        Pipeline* CreatePipeline(const PipelineCreateInfo& Specification);
        template<typename PipelineType> requires IsBaseOf<Pipeline, PipelineType>::Value
        PipelineType* CreatePipeline(const PipelineCreateInfo& CreateInfo)
        {
            return (PipelineType*)CreatePipeline(CreateInfo);
        }

        VertexBuffer* CreateVertexBuffer(const VertexBufferCreateInfo& CreateInfo);
        template<typename VertexBufferType> requires IsBaseOf<VertexBuffer, VertexBufferType>::Value
        VertexBufferType* CreateVertexBuffer(const VertexBufferCreateInfo& CreateInfo)
        {
            return (VertexBufferType*)CreateVertexBuffer(CreateInfo);
        }

        VertexBuffer* CreateVertexBuffer(const BufferView<Vertex>& Vertices);
        template<typename VertexBufferType> requires IsBaseOf<VertexBuffer, VertexBufferType>::Value
        VertexBufferType* CreateVertexBuffer(const BufferView<Vertex>& Vertices)
        {
            return (VertexBufferType*)CreateVertexBuffer(Vertices);
        }

        IndexBuffer* CreateIndexBuffer(const IndexBufferCreateInfo& CreateInfo);
        template<typename IndexBufferType> requires IsBaseOf<IndexBuffer, IndexBufferType>::Value
        IndexBufferType* CreateIndexBuffer(const IndexBufferCreateInfo& CreateInfo)
        {
            return (IndexBufferType*)CreateIndexBuffer(CreateInfo);
        }

        IndexBuffer* CreateIndexBuffer(const BufferView<u32>& Indices);
        template<typename IndexBufferType> requires IsBaseOf<IndexBuffer, IndexBufferType>::Value
        IndexBufferType* CreateIndexBuffer(const BufferView<u32>& Indices)
        {
            return (IndexBufferType*)CreateIndexBuffer(Indices);
        }

        UniformBuffer* CreateUniformBuffer(size_t Size);
        template<typename UniformBufferType> requires IsBaseOf<UniformBuffer, UniformBufferType>::Value
        UniformBufferType* CreateUniformBuffer(const size_t Size)
        {
            return (UniformBufferType*)CreateUniformBuffer(Size);
        }

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
        Window* m_Window = nullptr;
        GraphicsApi m_GraphicsApi = GraphicsApi::None;
    };
}
