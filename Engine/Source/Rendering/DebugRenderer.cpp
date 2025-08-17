#include "DebugRenderer.h"
#include "Buffer.h"
#include "Device.h"
#include "GraphicsPipeline.h"
#include "ShaderBindingSet.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Vulkan/Device.h"

namespace Nova
{
    struct LineVertex
    {
        Vector3 position;
        Color color;
    };

    Matrix4 s_ViewProjection;
    Array<LineVertex> s_Vertices;
    Array<uint32_t> s_Indices;
    uint32_t s_IndicesCount = 0;
    Ref<Rendering::Shader> s_Shader = nullptr;
    Ref<Rendering::Buffer> s_VertexBuffer = nullptr;
    Ref<Rendering::Buffer> s_IndexBuffer = nullptr;
    Ref<Rendering::GraphicsPipeline> s_Pipeline = nullptr;
    Ref<Rendering::ShaderBindingSet> s_BindingSet = nullptr;

    bool DebugRenderer::Initialize(const DebugRendererCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (!createInfo.shader) return false;

        s_Device = createInfo.device;
        s_Shader = createInfo.shader;

        Rendering::GraphicsPipelineCreateInfo pipelineInfo = Rendering::GraphicsPipelineCreateInfo();
        pipelineInfo.device = s_Device;
        pipelineInfo.shader = createInfo.shader;

        pipelineInfo.vertexInputInfo.layout.AddAttribute({"POSITION", Format::Vector3});
        pipelineInfo.vertexInputInfo.layout.AddAttribute({"COLOR", Format::Vector4});

        pipelineInfo.rasterizationInfo.cullMode = CullMode::None;
        pipelineInfo.rasterizationInfo.discardEnable = false;
        pipelineInfo.rasterizationInfo.frontFace = FrontFace::CounterClockwise;
        pipelineInfo.rasterizationInfo.lineWidth = 1.0f;
        pipelineInfo.rasterizationInfo.polygonMode = PolygonMode::Line;

        pipelineInfo.inputAssemblyInfo.topology = PrimitiveTopology::LineList;

        pipelineInfo.depthStencilInfo.depthTestEnable = true;
        pipelineInfo.depthStencilInfo.depthWriteEnable = true;
        pipelineInfo.depthStencilInfo.depthCompareOp = CompareOperation::Less;

        s_Pipeline = s_Device->CreateGraphicsPipeline(pipelineInfo);
        if (!s_Pipeline) return false;


        return true;
    }

    void DebugRenderer::Destroy()
    {
        s_Device->WaitIdle();
        s_Pipeline->Destroy();
        s_VertexBuffer->Destroy();
        s_IndexBuffer->Destroy();
    }

    void DebugRenderer::Begin(const Matrix4& viewProjection)
    {
        s_ViewProjection = viewProjection;
        s_Vertices.Clear();
        s_Indices.Clear();
        s_IndicesCount = 0;
    }

    void DebugRenderer::End()
    {
        if (s_Vertices.IsEmpty()) return;

        auto& cmdBuffer = s_Device.As<Vulkan::Device>()->GetCurrentCommandBuffer();
        cmdBuffer.UpdateBuffer(*s_VertexBuffer, 0, s_Vertices.Size(), s_Vertices.Data());
        cmdBuffer.UpdateBuffer(*s_IndexBuffer, 0, s_Indices.Size(), s_Indices.Data());
    }

    void DebugRenderer::Render(Rendering::CommandBuffer& cmdBuffer)
    {
        cmdBuffer.BindGraphicsPipeline(*s_Pipeline);
        cmdBuffer.BindVertexBuffer(*s_VertexBuffer, 0);
        cmdBuffer.BindIndexBuffer(*s_IndexBuffer, 0, Format::Uint32);
        cmdBuffer.PushConstants(s_Shader, ShaderStageFlagBits::Vertex, 0, sizeof(Matrix4), s_ViewProjection.ValuePtr());
        cmdBuffer.DrawIndexed(s_Indices.Count(), 0);
    }

    void DebugRenderer::DrawLine(const Vector3& start, const Vector3& end, const Color& color, float thickness)
    {
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

    void DebugRenderer::DrawSquare(const Vector3& position, const Vector3& size, const Color& color, float rotation, float thickness)
    {
        if (size.MagnitudeSquared() <= Math::Epsilon) return;

        const Vector3 posA = position - Vector3(-size.x * 0.5f, -size.y * 0.5f, 0.0f);
        const Vector3 posB = position - Vector3(-size.x * 0.5f, +size.y * 0.5f, 0.0f);
        const Vector3 posC = position - Vector3(+size.x * 0.5f, +size.y * 0.5f, 0.0f);
        const Vector3 posD = position - Vector3(+size.x * 0.5f, -size.y * 0.5f, 0.0f);

#if 1
        const LineVertex vertices[4]
        {
            LineVertex(posA, color),
            LineVertex(posB, color),
            LineVertex(posC, color),
            LineVertex(posD, color),
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
        s_IndicesCount += 8;
#else

        DrawLine(posA, posB, color, thickness);
        DrawLine(posB, posC, color, thickness);
        DrawLine(posC, posD, color, thickness);
        DrawLine(posD, posA, color, thickness);
#endif
    }

    void DebugRenderer::DrawCircle(const Vector3& position, float radius, const Color& color, float thickness)
    {
        if (radius <= Math::Epsilon) return;

        constexpr float angle = Math::Tau / 180;

        for (uint32_t i = 0; i < 30; i++)
        {
            const Vector3 point = Vector3(radius * Math::Cos((float)i * angle), radius * Math::Sin((float)i * angle), 0.0f);
            const LineVertex vertex(position + point, color);
            s_Vertices.Add(vertex);
        }


        const uint32_t indexBase = s_Vertices.Count();
        for (uint32_t i = 0; i < 30; i++)
        {
            if (i < 30 - 1)
            {
                const uint32_t indices[2]
                {
                    indexBase + i,
                    indexBase + i + 1,
                };
                s_Indices.AddRange(indices);
            } else
            {
                const uint32_t indices[2]
                {
                    indexBase + 30 - 1,
                    indexBase + 0,
                };
                s_Indices.AddRange(indices);
            }
        }
    }

    void DebugRenderer::DrawCapsule(const Vector3& position, float height, float radius, const Color& color, float thickness)
    {
    }
}
