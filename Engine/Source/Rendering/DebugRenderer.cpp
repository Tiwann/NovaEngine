#include "DebugRenderer.h"
#include "Buffer.h"
#include "RenderDevice.h"
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "ShaderBindingSet.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include "Runtime/Application.h"
#include "Vulkan/RenderDevice.h"

namespace Nova
{
    struct LineVertex
    {
        Vector3 position;
        Color color;
    };

    static Matrix4 s_ViewProjection = Matrix4::Identity;
    static Array<LineVertex> s_Vertices;
    static Array<uint32_t> s_Indices;
    static uint32_t s_IndicesCount = 0;
    static Ref<Shader> s_Shader = nullptr;
    static Ref<Buffer> s_VertexBuffer = nullptr;
    static Ref<Buffer> s_IndexBuffer = nullptr;
    static Ref<GraphicsPipeline> s_Pipeline = nullptr;
    static Ref<ShaderBindingSet> s_BindingSet = nullptr;
    static bool s_Begin = false;

    // CAREFUL THIS MIGHT INCREASE VERTEX COUNT
    static constexpr uint32_t CIRCLE_PRECISION = 60;

    bool DebugRenderer::Initialize(const DebugRendererCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (!createInfo.shader) return false;

        s_Device = createInfo.device;
        s_Shader = createInfo.shader;

        GraphicsPipelineCreateInfo pipelineInfo = GraphicsPipelineCreateInfo();
        pipelineInfo.device = s_Device;
        pipelineInfo.shader = s_Shader;
        pipelineInfo.colorAttachmentFormats = { Format::R8G8B8A8_UNORM };
        pipelineInfo.depthAttachmentFormat = Format::D32_FLOAT_S8_UINT;

        pipelineInfo.vertexInputInfo.layout.AddAttribute({"POSITION", Format::Vector3});
        pipelineInfo.vertexInputInfo.layout.AddAttribute({"COLOR", Format::Vector4});

        pipelineInfo.rasterizationInfo.cullMode = CullMode::FrontAndBackFaces;
        pipelineInfo.rasterizationInfo.discardEnable = false;
        pipelineInfo.rasterizationInfo.frontFace = FrontFace::CounterClockwise;
        pipelineInfo.rasterizationInfo.lineWidth = 3.0f;
        pipelineInfo.rasterizationInfo.polygonMode = PolygonMode::Line;

        pipelineInfo.inputAssemblyInfo.topology = PrimitiveTopology::LineList;

        pipelineInfo.depthStencilInfo.depthTestEnable = false;
        pipelineInfo.depthStencilInfo.depthWriteEnable = false;
        pipelineInfo.depthStencilInfo.depthCompareOp = CompareOperation::Less;

        pipelineInfo.multisampleInfo.sampleCount = 8;

        s_Pipeline = s_Device->CreateGraphicsPipeline(pipelineInfo);
        if (!s_Pipeline) return false;

        BufferCreateInfo vertexBufferCreateInfo;
        vertexBufferCreateInfo.device = s_Device;
        vertexBufferCreateInfo.usage = BufferUsage::VertexBuffer;
        vertexBufferCreateInfo.size = createInfo.maxVertices * sizeof(LineVertex);
        s_VertexBuffer = s_Device->CreateBuffer(vertexBufferCreateInfo);
        if (!s_VertexBuffer) return false;

        BufferCreateInfo indexBufferCreateInfo;
        indexBufferCreateInfo.device = s_Device;
        indexBufferCreateInfo.usage = BufferUsage::IndexBuffer;
        indexBufferCreateInfo.size = 2 * createInfo.maxVertices * sizeof(LineVertex);
        s_IndexBuffer = s_Device->CreateBuffer(indexBufferCreateInfo);
        if (!s_IndexBuffer) return false;

        return true;
    }

    void DebugRenderer::Destroy()
    {
        if (s_Device) s_Device->WaitIdle();
        if (s_Pipeline) s_Pipeline->Destroy();
        if (s_VertexBuffer) s_VertexBuffer->Destroy();
        if (s_IndexBuffer) s_IndexBuffer->Destroy();
    }

    void DebugRenderer::Begin(const Matrix4& viewProjection)
    {
        NOVA_ASSERT(!s_Begin, "DebugRenderer::Begin/End call mismatch!");
        s_ViewProjection = viewProjection;
        s_Vertices.Clear();
        s_Indices.Clear();
        s_IndicesCount = 0;
        s_Begin = true;
    }

    void DebugRenderer::End(CommandBuffer& cmdBuffer)
    {
        NOVA_ASSERT(s_Begin, "DebugRenderer::End/End call mismatch!");
        if (s_Vertices.IsEmpty())
        {
            s_Begin = false;
            return;
        }

        cmdBuffer.UpdateBuffer(*s_VertexBuffer, 0, s_Vertices.Size(), s_Vertices.Data());
        cmdBuffer.UpdateBuffer(*s_IndexBuffer, 0, s_Indices.Size(), s_Indices.Data());
        s_Begin = false;
    }

    void DebugRenderer::Render(CommandBuffer& cmdBuffer)
    {
        if (s_Vertices.IsEmpty()) return;

        Application& application = Application::GetCurrentApplication();
        const auto window = application.GetWindow();
        const auto viewport = window->GetBounds();

        cmdBuffer.BindGraphicsPipeline(*s_Pipeline);
        cmdBuffer.BindVertexBuffer(*s_VertexBuffer, 0);
        cmdBuffer.BindIndexBuffer(*s_IndexBuffer, 0, Format::Uint32);
        cmdBuffer.PushConstants(*s_Shader, ShaderStageFlagBits::Vertex | ShaderStageFlagBits::Fragment, 0, sizeof(Matrix4), s_ViewProjection.ValuePtr());
        cmdBuffer.SetViewport(viewport.x, viewport.y, viewport.width, viewport.height, 0.0f, 1.0f);
        cmdBuffer.SetScissor(viewport.x, viewport.y, viewport.width, viewport.height);
        cmdBuffer.DrawIndexed(s_Indices.Count(), 1, 0, 0, 0);
    }

    void DebugRenderer::DrawLine(const Vector3& start, const Vector3& end, const Color& color, float thickness)
    {
        NOVA_ASSERT(s_Begin, "Cannot draw outside a DebugRenderer::Begin/End scope!");
        const Vector3 line = end - start;
        if (line.MagnitudeSquared() <= Math::Epsilon) return;

        const LineVertex vertices[2]
        {
            LineVertex(start, color),
            LineVertex(end, color),
        };

        s_Vertices.AddRange(vertices);

        const uint32_t indices[2] { s_IndicesCount + 0, s_IndicesCount + 1 };
        s_Indices.AddRange(indices);

        s_IndicesCount += 2;
    }

    void DebugRenderer::DrawSquare(const Vector3& position, const Quaternion& rotation, const Vector3& size, const Color& color, float thickness)
    {
        NOVA_ASSERT(s_Begin, "Cannot draw outside a DebugRenderer::Begin/End scope!");
        if (size.MagnitudeSquared() <= Math::Epsilon) return;

        const Vector3 posA = position + Vector3(-size.x * 0.5f, -size.y * 0.5f, 0.0f);
        const Vector3 posB = position + Vector3(-size.x * 0.5f, +size.y * 0.5f, 0.0f);
        const Vector3 posC = position + Vector3(+size.x * 0.5f, +size.y * 0.5f, 0.0f);
        const Vector3 posD = position + Vector3(+size.x * 0.5f, -size.y * 0.5f, 0.0f);

#if 1
        const LineVertex vertices[4]
        {
            LineVertex(rotation * posA, color),
            LineVertex(rotation * posB, color),
            LineVertex(rotation * posC, color),
            LineVertex(rotation * posD, color),
        };

        s_Vertices.AddRange(vertices);

        const uint32_t indices[8]
        {
            s_IndicesCount + 0,
            s_IndicesCount + 1,
            s_IndicesCount + 1,
            s_IndicesCount + 2,
            s_IndicesCount + 2,
            s_IndicesCount + 3,
            s_IndicesCount + 3,
            s_IndicesCount + 0,
        };
        s_Indices.AddRange(indices);
        s_IndicesCount += 4;
#else

        DrawLine(posA, posB, color, thickness);
        DrawLine(posB, posC, color, thickness);
        DrawLine(posC, posD, color, thickness);
        DrawLine(posD, posA, color, thickness);
#endif
    }

    void DebugRenderer::DrawCircle(const Vector3& position, const Quaternion& rotation, const float radius, const Color& color, float thickness)
    {
        NOVA_ASSERT(s_Begin, "Cannot draw outside a DebugRenderer::Begin/End scope!");
        if (radius <= Math::Epsilon) return;

        constexpr float angle = Math::Tau / CIRCLE_PRECISION;

        for (uint32_t i = 0; i < CIRCLE_PRECISION; i++)
        {
            const Vector3 point = position + Vector3(radius * Math::Cos((float)i * angle), radius * Math::Sin((float)i * angle), 0.0f);
            const LineVertex vertex(rotation * point, color);
            s_Vertices.Add(vertex);
        }

        for (uint32_t i = 0; i < CIRCLE_PRECISION; i++)
        {
            if (i < CIRCLE_PRECISION - 1)
            {
                const uint32_t indices[2]
                {
                    s_IndicesCount + i,
                    s_IndicesCount + i + 1,
                };
                s_Indices.AddRange(indices);
            } else
            {
                const uint32_t indices[2]
                {
                    s_IndicesCount + CIRCLE_PRECISION - 1,
                    s_IndicesCount + 0,
                };
                s_Indices.AddRange(indices);
            }
        }

        s_IndicesCount += s_Vertices.Count();
    }

    void DebugRenderer::DrawCapsule(const Vector3& position, float height, float radius, const Color& color, float thickness)
    {
        NOVA_ASSERT(s_Begin, "Cannot draw outside a DebugRenderer::Begin/End scope!");
    }
}
