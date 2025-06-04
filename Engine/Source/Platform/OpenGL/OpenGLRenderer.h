#pragma once
#include "OpenGLRendererTypeConvertor.h"
#include "Rendering/CullMode.h"
#include "Rendering/Renderer.h"
#include "Runtime/LogCategory.h"
#include "Runtime/LogVerbosity.h"
#include "Rendering/Filter.h"
#include "Rendering/PrimitiveTopology.h"
#include "Rendering/SamplerAddressMode.h"
#include "Runtime/Format.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(OpenGL, "OPENGL");

typedef Nova::u32 GLenum;

namespace Nova
{
    class Shader;
    class OpenGLPipeline;
    
    class OpenGLRenderer : public Renderer
    {
    public:
        explicit OpenGLRenderer(Application* Owner) : Renderer(Owner, GraphicsApi::OpenGL) {}
        ~OpenGLRenderer() override = default;

        bool Initialize(const RendererCreateInfo& CreateInfo) override;
        void Destroy() override;
        void ClearDepth(float Depth) override;
        void ClearColor(const Color& color) override;
        void Present() override;
        bool BeginFrame() override;
        void BeginRendering() override;
        void EndRendering() override;
        void EndFrame() override;
        void SetViewport(const Viewport& Viewport) override;
        void SetScissor(const Scissor& Scissor) override;
        void DrawIndexed(size_t IndexCount, size_t Offset) override;
        void SetCullMode(CullMode Mode) override;
        void SetDepthCompareOperation(CompareOperation DepthFunction) override;
        static GLenum ConvertFormat(Format Format);
        static GLenum GetFormatType(Format Format);
        void SetBlendFunction(BlendFactor Source, BlendFactor Destination, BlendOperation Operation) override;
        void SetBlendFunction(BlendFactor ColorSource, BlendFactor ColorDest, BlendOperation ColorOperation, BlendFactor AlphaSource, BlendFactor AlphaDest, BlendOperation AlphaOperation) override;
        void SetBlending(bool Enabled) override;
        void BindPipeline(Pipeline* Pipeline) override;
        void UpdateUniformBuffer(UniformBuffer* Buffer, u64 Offset, u64 Size, const void* Data) override;
        void BindVertexBuffer(VertexBuffer* Buffer, u64 Offset) override;
        void BindIndexBuffer(IndexBuffer* Buffer, u64 Offset) override;
        OpenGLRendererTypeConvertor Convertor;
    protected:
        static String GetDebugSourceName(u32 Source);
        static Verbosity GetDebugVerbosity(u32 Severity);

    protected:
        OpenGLPipeline* m_BoundPipeline = nullptr;
    };
}
