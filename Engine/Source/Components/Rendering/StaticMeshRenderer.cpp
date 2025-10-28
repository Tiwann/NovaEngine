#include "StaticMeshRenderer.h"
#include "LightComponent.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Containers/Std140Buffer.h"
#include "Runtime/StaticMesh.h"
#include "Rendering/GraphicsPipeline.h"
#include "Runtime/AssetDatabase.h"
#include "Rendering/Shader.h"
#include "Runtime/Application.h"
#include "Rendering/Device.h"
#include "Runtime/Scene.h"
#include "Runtime/Window.h"
#include "Math/Matrix3x4.h"
#include "Math/Vector3.h"
#include "Rendering/Buffer.h"
#include "Rendering/CommandBuffer.h"
#include "Rendering/ShaderBindingSet.h"

namespace Nova
{
    struct alignas(16) SceneData
    {
        Matrix4 modelMatrix;
        Matrix4 viewMatrix;
        Matrix4 projectionMatrix;
        Matrix3x4 normalMatrix;
        Vector3 cameraViewDirection;
        float directionalLightIntensity;
        Vector3 directionalLightColor;
        float padding2;
        Vector3 directionalLightDirection;
        float ambientLightIntensity;
        Vector3 ambientLightColor;
    };

    StaticMeshRenderer::StaticMeshRenderer(Entity* owner) : Component(owner, "Static Mesh Component")
    {
    }

    void StaticMeshRenderer::OnInit()
    {
        Component::OnInit();

        const Entity* owner = GetOwner();
        const Scene* scene = owner->GetOwner();
        const Application* application = scene->GetOwner();
        Ref<Rendering::Device> device = application->GetDevice();
        const Window* window = application->GetWindow();
        const float width = window->GetWidth();
        const float height = window->GetHeight();

        const AssetDatabase& assetDatabase = application->GetAssetDatabase();
        m_Shader = assetDatabase.Get<Rendering::Shader>("BlinnPhong");

        const Array vertexAttributes
        {
            VertexAttribute{"POSITION", Format::Vector3},
            VertexAttribute{"TEXCOORDINATE", Format::Vector2},
            VertexAttribute{"NORMAL", Format::Vector3},
            VertexAttribute{"COLOR", Format::Vector4},
        };

        const Rendering::GraphicsPipelineCreateInfo pipelineCreateInfo = Rendering::GraphicsPipelineCreateInfo()
            .setDevice(device)
            .setShader(m_Shader)
            .setPrimitiveTopology(PrimitiveTopology::TriangleList)
            .setVertexLayout(vertexAttributes)
            .setCullMode(CullMode::BackFace)
            .setFrontFace(FrontFace::CounterClockwise)
            .setPolygonMode(PolygonMode::Fill)
            .setDepthStencilInfo({
                .depthTestEnable = true,
                .depthWriteEnable = true,
                .stencilTestEnable = false,
                .depthCompareOp = CompareOperation::Less
            })
            .setViewportInfo({0, 0, (uint32_t)width, (uint32_t)height, 0.0f, 1.0f})
            .setScissorInfo({0, 0, (uint32_t)width, (uint32_t)height});

        m_Pipeline = device->CreateGraphicsPipeline(pipelineCreateInfo);

        Rendering::BufferCreateInfo uniformBufferCreateInfo;
        uniformBufferCreateInfo.device = device;
        uniformBufferCreateInfo.size = sizeof(SceneData);
        uniformBufferCreateInfo.usage = Rendering::BufferUsage::UniformBuffer;
        m_SceneUniformBuffer = device->CreateBuffer(uniformBufferCreateInfo);

        m_BindingSet = m_Shader->CreateBindingSet();
        m_BindingSet->BindBuffer(0, m_SceneUniformBuffer, 0, ~0);
    }

    void StaticMeshRenderer::OnDestroy()
    {
        Component::OnDestroy();
        m_SceneUniformBuffer->Destroy();
        m_Pipeline->Destroy();
    }

    void StaticMeshRenderer::OnPreRender(Rendering::CommandBuffer& cmdBuffer)
    {
        Component::OnPreRender(cmdBuffer);
        if (!m_StaticMesh) return;

        Entity* owner = GetOwner();
        Scene* scene = owner->GetOwner();
        Camera* camera = scene->GetFirstComponent<Camera>();
        if (!camera) return;

        Transform* entityTransform = owner->GetTransform();
        Transform* cameraTransform = camera->GetTransform();

        const Matrix4& modelMatrix = entityTransform->GetWorldSpaceMatrix();
        const Matrix4& viewMatrix = camera->GetViewMatrix();
        const Matrix4& projectionMatrix = camera->GetProjectionMatrix();
        const Matrix3& normalMatrix = entityTransform->GetWorldSpaceNormalMatrix();

        const Vector3& cameraPosition = cameraTransform->GetPosition();
        const Vector3& entityPosition = entityTransform->GetPosition();
        const Vector3 cameraViewDirection = Math::Normalize(cameraPosition - entityPosition);

        const Array<LightComponent*> allLights = scene->GetAllComponents<LightComponent>();
        LightComponent** dirLight = allLights.Where([](const LightComponent* light) { return light->GetType() == LightType::Directional; }).First();
        LightComponent** ambLight = allLights.Where([](const LightComponent* light) { return light->GetType() == LightType::Ambient; }).First();

        const Color& dirLightColor = dirLight ? (*dirLight)->GetColor() : Color::Black;
        const float dirLightIntensity = dirLight ? (*dirLight)->GetIntensity() : 0.0f;
        const Vector3 dirLightDir = dirLight ? (*dirLight)->GetTransform()->GetForwardVector() : Vector3::Zero;
        const Color& ambLightColor = ambLight ? (*ambLight)->GetColor() : Color::Black;
        const float ambLightIntensity = ambLight ? (*ambLight)->GetIntensity() : 0.0f;

        uint8_t buffer[512];

        Std140Buffer bufferWriter(buffer, std::size(buffer));
        bufferWriter.WriteMatrix4(modelMatrix);
        bufferWriter.WriteMatrix4(viewMatrix);
        bufferWriter.WriteMatrix4(projectionMatrix);
        bufferWriter.WriteMatrix3Aligned(normalMatrix);
        bufferWriter.WriteVector4(Vector4(cameraViewDirection));
        bufferWriter.WriteVector4(Vector4(dirLightColor.r, dirLightColor.g, dirLightColor.b, dirLightIntensity));
        bufferWriter.WriteVector4(Vector4(ambLightColor.r, ambLightColor.g, ambLightColor.g, ambLightIntensity));
        bufferWriter.WriteVector3Aligned(dirLightDir);


        cmdBuffer.UpdateBuffer(*m_SceneUniformBuffer, 0, bufferWriter.Tell(), buffer);
    }

    void StaticMeshRenderer::OnRender(Rendering::CommandBuffer& cmdBuffer)
    {
        Component::OnRender(cmdBuffer);
        if (!m_StaticMesh)
            return;

        if (m_StaticMesh->GetSubMeshes().IsEmpty())
            return;

        // THIS NEEDS TO BE OPTIMIZED BY FILTER THE AVAILABLE LIGHTS
        Entity* owner = GetOwner();
        const Scene* scene = owner->GetOwner();
        const Application* application = scene->GetOwner();
        const Window* window = application->GetWindow();

        const Camera* camera = scene->GetFirstComponent<Camera>();
        if (!camera) return;

        const float width = window->GetWidth();
        const float height = window->GetHeight();

        cmdBuffer.BindGraphicsPipeline(*m_Pipeline);
        cmdBuffer.BindShaderBindingSet(*m_Shader, *m_BindingSet);
        cmdBuffer.SetViewport(0.0f, 0.0f, width, width, 0.0f, 1.0f);
        cmdBuffer.SetScissor(0, 0, (int32_t)width, (int32_t)height);

        for (const SubMeshInfo& subMesh : m_StaticMesh->GetSubMeshes())
        {
            cmdBuffer.BindVertexBuffer(*m_StaticMesh->GetVertexBuffer(), subMesh.vertexBufferOffset);
            cmdBuffer.BindIndexBuffer(*m_StaticMesh->GetIndexBuffer(), subMesh.indexBufferOffset, Format::Uint32);
            cmdBuffer.DrawIndexed(subMesh.indexBufferSize / sizeof(uint32_t), 0);
        }
    }

    Ref<StaticMesh> StaticMeshRenderer::GetStaticMesh() const
    {
        return m_StaticMesh;
    }

    void StaticMeshRenderer::SetStaticMesh(Ref<StaticMesh> newMesh)
    {
        const Entity* owner = GetOwner();
        const Scene* scene = owner->GetOwner();
        const Application* application = scene->GetOwner();
        const Ref<Rendering::Device> device = application->GetDevice();
        device->WaitIdle();

        m_StaticMesh = newMesh;
    }
}
