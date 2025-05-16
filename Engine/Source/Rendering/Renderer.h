#pragma once
#include "Runtime/Object.h"
#include "Containers/Buffer.h"
#include "Containers/String.h"
#include "Runtime/Filesystem.h"
#include "GraphicsApi.h"
#include "CullMode.h"
#include "BlendOperation.h"
#include "BlendFactor.h"
#include "CompareOperation.h"
#include "Fence.h"
#include "PresentMode.h"
#include "RenderTarget.h"
#include "Scissor.h"
#include "Vertex.h"

namespace Nova
{
    class RenderTarget;
    class UniformBuffer;
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
    struct PipelineCreateInfo;
    class VertexBuffer;
    class Swapchain;
    struct SwapchainCreateInfo;
    struct Viewport;

    class CommandPool;
    struct CommandPoolCreateInfo;
    class CommandBuffer;

    class Renderer : public Object
    {
    public:
        Renderer(Application* Owner, const GraphicsApi& GraphicsApi) : m_Application(Owner), m_GraphicsApi(GraphicsApi) {}
        static Renderer* Create(Application* Owner, const GraphicsApi& GraphicsApi);

        bool ShouldRecreateSwapchain = false;

        ~Renderer() override = default;
        virtual bool Initialize() = 0;
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
        virtual void DrawIndexed(size_t IndexCount, u64 Offset) = 0;
        virtual void SetCullMode(CullMode Mode) = 0;
        virtual void SetDepthCompareOperation(CompareOperation DepthFunction) = 0;
        virtual void SetBlendFunction(BlendFactor ColorSource, BlendFactor ColorDest, BlendOperation ColorOperation, BlendFactor AlphaSource, BlendFactor AlphaDest, BlendOperation AlphaOperation) = 0;
        virtual void SetBlendFunction(BlendFactor Source, BlendFactor Destination, BlendOperation Operation) = 0;
        virtual void SetBlending(bool Enabled) = 0;
        virtual void BindPipeline(Pipeline* Pipeline) = 0;
        virtual void UpdateUniformBuffer(UniformBuffer* Buffer, u64 Offset, u64 Size, const void* Data) = 0;
        virtual void BindVertexBuffer(VertexBuffer* Buffer, u64 Offset) = 0;
        virtual void BindIndexBuffer(IndexBuffer* Buffer, u64 Offset) = 0;
        virtual PresentMode GetPresentMode() = 0;

        Fence* CreateFence(const FenceCreateInfo& CreateInfo);
        RenderTarget* CreateRenderTarget(const RenderTargetCreateInfo& CreateInfo);
        CommandPool* CreateCommandPool(const CommandPoolCreateInfo& CreateInfo);
        Swapchain* CreateSwapchain(const SwapchainCreateInfo& CreateInfo);
        Shader* CreateShader(const String& Name, const Path& Filepath);
        Pipeline* CreatePipeline(const PipelineCreateInfo& CreateInfo);
        VertexBuffer* CreateVertexBuffer();
        VertexBuffer* CreateVertexBuffer(const BufferView<Vertex>& Vertices);
        IndexBuffer* CreateIndexBuffer();
        IndexBuffer* CreateIndexBuffer(const BufferView<u32>& Indices);
        UniformBuffer* CreateUniformBuffer(size_t Size);

        void SetCurrentCamera(Camera* Camera);
        Camera* GetCurrentCamera();
        const Camera* GetCurrentCamera() const;

        RenderTarget* GetRenderTarget() const;
        void SetRenderTarget(RenderTarget* RenderTarget);
        GraphicsApi GetGraphicsApi() const;
        Application* GetOwner();
        const Application* GetOwner() const;
    protected:
        Camera* m_CurrentCamera = nullptr;
        Application* m_Application = nullptr;
        RenderTarget* m_RenderTarget = nullptr;
        GraphicsApi m_GraphicsApi;
    };
}
