#pragma once
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
    
    class OpenGLRenderer : public Renderer
    {
    public:
        OpenGLRenderer(Application* Owner) : Renderer(Owner, GraphicsApi::OpenGL) {}
        ~OpenGLRenderer() override = default;

        bool Initialize() override;
        void Destroy() override;
        void ClearDepth(float Depth) override;
        void ClearColor(const Color& color) override;
        void Present() override;
        bool BeginFrame() override;
        void EndFrame() override;
        void SetViewportRect(Vector2 Position, Vector2 Size) override;
        void Draw(VertexArray* VAO, u32 NumVert, Shader* Shader) override;
        void DrawIndexed(VertexArray* VertexArray, VertexBuffer* VertexBuffer, IndexBuffer* IndexBuffer, Shader* Shader) override;
        void SetCullMode(CullMode Mode) override;
        void SetDepthCompareOperation(CompareOperation DepthFunction) override;
        void SetBlendFunction(BlendFactor Source, BlendFactor Destination, BlendOperation Operation) override;
        void SetBlendFunction(BlendFactor ColorSource, BlendFactor ColorDest, BlendOperation ColorOperation, BlendFactor AlphaSource, BlendFactor AlphaDest, BlendOperation AlphaOperation) override;
        void SetBlending(bool Enabled) override;
        void BindPipeline(const Pipeline* Pipeline) override;


        static GLenum ConvertPrimitiveTopology(PrimitiveTopology Mode);
        static GLenum ConvertBlendFactor(BlendFactor Mode);
        static GLenum ConvertBlendOperation(BlendOperation Operation);
        static GLenum ConvertFilter(Filter Filter);
        static GLenum ConvertSamplerAddressMode(SamplerAddressMode AddressMode);
        static GLenum ConvertFormat(Format Format);
        static GLenum GetFormatType(Format Format);
    protected:
        static String GetDebugSourceName(u32 Source);
        static Verbosity GetDebugVerbosity(u32 Severity);
    };
}
