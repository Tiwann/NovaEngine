#include "Renderer2D.h"

#include "Buffer.h"
#include "CommandBuffer.h"
#include "Fence.h"
#include "GraphicsPipeline.h"
#include "Queue.h"
#include "Shader.h"
#include "ShaderEntryPoint.h"
#include "Components/Camera.h"
#include "Containers/Array.h"
#include "Math/Matrix2.h"
#include "Math/Quaternion.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Runtime/Application.h"
#include "Runtime/Assertion.h"
#include "Runtime/Common.h"
#include "Runtime/Path.h"
#include "Utils/BufferUtils.h"

namespace Nova
{
    enum class QuadMode
    {
        Quad,
        Ellipse
    };

    struct QuadVertex
    {
        Vector2 position;
        Vector2 uv;
        Vector4 color;
        QuadMode quadMode;
    };

    static constexpr uint32_t MAX_QUAD = 1024;
    static bool BeginDrawing = false;
    static bool ReadyToRender = false;
    static Array<QuadVertex> QuadVertices;
    static Array<uint32_t> QuadIndices;

    bool Renderer2D::Initialize(Ref<RenderDevice> renderDevice)
    {
        if (!renderDevice) return false;
        m_RenderDevice = renderDevice;

        ShaderCreateInfo shaderCreateInfo;
        shaderCreateInfo.entryPoints.AddRange(ShaderEntryPoint::Default(), 2);
        shaderCreateInfo.moduleInfo = { "Renderer2D", Path::GetEngineAssetPath("Shaders/Renderer2D.slang") };
        m_Shader = m_RenderDevice->CreateShader(shaderCreateInfo);
        if (!m_Shader) return false;


        VertexLayout vertexLayout;
        vertexLayout.AddInputBinding(0, VertexInputRate::Vertex);
        vertexLayout.AddInputAttribute("POSITION", ShaderDataType::Float2, 0);
        vertexLayout.AddInputAttribute("TEXCOORD", ShaderDataType::Float2, 0);
        vertexLayout.AddInputAttribute("COLOR", ShaderDataType::Float4, 0);
        vertexLayout.AddInputAttribute("MODE", ShaderDataType::UInt, 0);

        GraphicsPipelineCreateInfo gpCreateInfo;
        gpCreateInfo.device = m_RenderDevice;
        gpCreateInfo.shader = m_Shader;
        gpCreateInfo.colorAttachmentFormats = { Format::R8G8B8A8_SRGB };
        gpCreateInfo.colorBlendStates = {ColorBlendState(true, BlendFunction::AlphaBlend(), ColorChannelFlags::All())};
        gpCreateInfo.vertexInputState = CreateInputStateFromVertexLayout(vertexLayout);
        gpCreateInfo.depthStencilState.depthTestEnable = false;
        gpCreateInfo.depthStencilState.depthWriteEnable = false;
        gpCreateInfo.depthStencilState.stencilTestEnable = false;
        gpCreateInfo.rasterizationState.cullMode = CullMode::FrontFace;

        gpCreateInfo.SetMultisampleInfo({8});
        m_Pipeline = m_RenderDevice->CreateGraphicsPipeline(gpCreateInfo);
        if (!m_Pipeline) return false;

        BufferCreateInfo vbCreateInfo;
        vbCreateInfo.mapped = true;
        vbCreateInfo.device = m_RenderDevice;
        vbCreateInfo.usage = BufferUsage::VertexBuffer;
        vbCreateInfo.size = MAX_QUAD * 4 * sizeof(QuadVertex);
        m_VertexBuffer = m_RenderDevice->CreateBuffer(vbCreateInfo);
        if (!m_VertexBuffer) return false;

        BufferCreateInfo ibCreateInfo;
        ibCreateInfo.mapped = true;
        ibCreateInfo.device = m_RenderDevice;
        ibCreateInfo.usage = BufferUsage::IndexBuffer;
        ibCreateInfo.size = MAX_QUAD * 6 * sizeof(uint32_t);
        m_IndexBuffer = m_RenderDevice->CreateBuffer(ibCreateInfo);
        if (!m_IndexBuffer) return false;
        return true;
    }

    void Renderer2D::Destroy()
    {
        m_RenderDevice = nullptr;
        m_Shader->Destroy();
        m_Pipeline->Destroy();
        m_VertexBuffer->Destroy();
        m_IndexBuffer->Destroy();
        QuadVertices.Free();
        QuadIndices.Free();
    }

    void Renderer2D::Begin()
    {
        NOVA_ASSERT(!BeginDrawing, "Begin/End mismatch");
        BeginDrawing = true;
        ReadyToRender = false;
        QuadVertices.Clear();
        QuadIndices.Clear();
    }

    void Renderer2D::End()
    {
        NOVA_ASSERT(BeginDrawing, "Begin/End mismatch");

        void* vertexMapped = m_VertexBuffer->Map();
        Memory::Memcpy(vertexMapped, QuadVertices.Data(), QuadVertices.Size());
        m_VertexBuffer->Unmap(vertexMapped);

        void* indexMapped = m_IndexBuffer->Map();
        Memory::Memcpy(indexMapped, QuadIndices.Data(), QuadIndices.Size());
        m_IndexBuffer->Unmap(indexMapped);

        BeginDrawing = false;
        ReadyToRender = true;
    }

    void Renderer2D::Render(CommandBuffer& cmdBuffer)
    {
        NOVA_ASSERT(ReadyToRender, "not ready to render yet!!");

        const Application& application = Application::GetCurrentApplication();
        const uint32_t windowWidth = application.GetWindowWidth();
        const uint32_t windowHeight = application.GetWindowHeight();

        Matrix4 projection = Math::OrthographicTopLeft(windowWidth, windowHeight, 1.0f, -1.0, 1.0f);
        cmdBuffer.PushConstants(*m_Shader, ShaderStageFlagBits::Vertex, 0, sizeof(Matrix4), projection.ValuePtr());
        cmdBuffer.BindVertexBuffer(*m_VertexBuffer, 0);
        cmdBuffer.BindIndexBuffer(*m_IndexBuffer, 0, Format::R32_UINT);
        cmdBuffer.BindGraphicsPipeline(*m_Pipeline);
        cmdBuffer.SetViewport(0.0f, (float)windowHeight, (float)windowWidth, -(float)windowHeight, 0.0f, 1.0f);
        cmdBuffer.SetScissor(0, 0, windowWidth, windowHeight);
        cmdBuffer.DrawIndexed(QuadIndices.Count(), 1, 0, 0, 0);
        ReadyToRender = false;
    }

    static void AddQuad(const Vector2& position, const Vector2& size, float rotation, const Color& color, QuadMode quadMode)
    {
        const Matrix2 rotationMatrix = Math::Rotate(Matrix2::Identity, rotation);

        const Vector2 topLeft     = Vector2(0.0f, size.y);
        const Vector2 topRight    = Vector2(size.x, size.y);
        const Vector2 bottomLeft  = Vector2(0.0f, 0.0f);
        const Vector2 bottomRight = Vector2(size.x, 0.0f);

        const uint32_t lastVertexCount = QuadVertices.Count();

        const QuadVertex quadVertices[]
        {
            { position + rotationMatrix * bottomLeft,  Vector2(0.0f, 1.0f), color, quadMode },
            { position + rotationMatrix * topLeft,     Vector2(1.0f, 1.0f), color, quadMode },
            { position + rotationMatrix * topRight,    Vector2(1.0f, 0.0f), color, quadMode },
            { position + rotationMatrix * bottomRight, Vector2(0.0f, 0.0f), color, quadMode },
        };
        QuadVertices.AddRange(quadVertices);

        const uint32_t quadIndices[]
        {
            0 + lastVertexCount, 2 + lastVertexCount, 1 + lastVertexCount,
            0 + lastVertexCount, 3 + lastVertexCount, 2 + lastVertexCount
        };
        QuadIndices.AddRange(quadIndices);
    }


    void Renderer2D::DrawQuad(const Vector2& position, const Vector2& size, float rotation, const Color& color)
    {
        AddQuad(position, size, rotation, color, QuadMode::Quad);
    }

    void Renderer2D::DrawQuad(const Rect2D<float>& rect, float rotation, const Color& color)
    {
        const Vector2 position = { rect.x, rect.y };
        const Vector2 size = { rect.width, rect.height };
        DrawQuad(position, size, rotation, color);
    }

    void Renderer2D::DrawEllipse(const Vector2& position, const Vector2& size, float rotation, const Color& color)
    {
        AddQuad(position, size, rotation, color, QuadMode::Ellipse);
    }

    void Renderer2D::DrawEllipse(const Rect2D<float>& rect, float rotation, const Color& color)
    {
        const Vector2 position = { rect.x, rect.y };
        const Vector2 size = { rect.width, rect.height };
        DrawEllipse(position, size, rotation, color);
    }

    void Renderer2D::DrawCircle(const Vector2& position, float radius, const Color& color)
    {
        DrawEllipse(position, {radius, radius}, 0.0f, color);
    }

    void Renderer2D::DrawText(StringView text, const Vector2& position, float rotation, const Color& color)
    {

    }
}
