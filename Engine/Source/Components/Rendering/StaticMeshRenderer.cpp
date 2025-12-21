#include "StaticMeshRenderer.h"
#include "LightComponent.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
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
        Vector3 cameraPosition;
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
        Application* application = scene->GetOwner();
        Ref<Device> device = application->GetDevice();

        const AssetDatabase& assetDatabase = application->GetAssetDatabase();
        m_Shader = assetDatabase.Get<Shader>("BlinnPhongShader");

        const Array vertexAttributes
        {
            VertexAttribute{"POSITION", Format::Vector3},
            VertexAttribute{"TEXCOORDINATE", Format::Vector2},
            VertexAttribute{"NORMAL", Format::Vector3},
            VertexAttribute{"TANGENT", Format::Vector3},
            VertexAttribute{"COLOR", Format::Vector4},
        };

        const GraphicsPipelineCreateInfo pipelineCreateInfo = GraphicsPipelineCreateInfo()
        .SetDevice(device)
        .SetShader(m_Shader)
        .SetRenderPass(application->GetRenderPass())
        .SetVertexLayout(vertexAttributes)
        .SetMultisampleInfo({8})
        .SetDepthStencilInfo({
            .depthTestEnable = true,
            .depthWriteEnable = true,
        });

        m_Pipeline = device->CreateGraphicsPipeline(pipelineCreateInfo);

        BufferCreateInfo uniformBufferCreateInfo;
        uniformBufferCreateInfo.device = device;
        uniformBufferCreateInfo.size = sizeof(SceneData);
        uniformBufferCreateInfo.usage = BufferUsage::UniformBuffer;
        m_SceneUniformBuffer = device->CreateBuffer(uniformBufferCreateInfo);

        const SamplerCreateInfo samplerCreateInfo = SamplerCreateInfo()
        .WithAddressMode(SamplerAddressMode::Repeat)
        .WithFilter(Filter::Linear, Filter::Linear)
        .WithLODRange(0.0f, 1.0f);
        m_Sampler = device->CreateSampler(samplerCreateInfo);

        m_BindingSet = m_Shader->CreateBindingSet();
        m_BindingSet->BindBuffer(0, *m_SceneUniformBuffer, 0, ~0);
        m_BindingSet->BindSampler(1, *m_Sampler);
    }

    void StaticMeshRenderer::OnDestroy()
    {
        Component::OnDestroy();
        const Entity* owner = GetOwner();
        const Scene* scene = owner->GetOwner();
        Application* application = scene->GetOwner();
        Ref<Device> device = application->GetDevice();
        device->WaitIdle();

        m_BindingSet->Destroy();
        m_SceneUniformBuffer->Destroy();
        m_Pipeline->Destroy();
        m_Sampler->Destroy();
    }

    void StaticMeshRenderer::OnPreRender(CommandBuffer& cmdBuffer)
    {
        Component::OnPreRender(cmdBuffer);
        if (!m_StaticMesh) return;

        Entity* owner = GetOwner();
        Scene* scene = owner->GetOwner();
        Camera* camera = scene->GetFirstComponent<Camera>();
        if (!camera) return;

        Transform* entityTransform = owner->GetTransform();
        const Transform* cameraTransform = camera->GetTransform();

        const Matrix4& modelMatrix = entityTransform->GetWorldSpaceMatrix();
        const Matrix4& viewMatrix = camera->GetViewMatrix();
        const Matrix4& projectionMatrix = camera->GetProjectionMatrix();
        const Matrix3& normalMatrix = entityTransform->GetWorldSpaceNormalMatrix();

        const Vector3& cameraPosition = cameraTransform->GetPosition();
        const Vector3& entityPosition = entityTransform->GetPosition();

        const Array<LightComponent*> allLights = scene->GetAllComponents<LightComponent>();
        Array<LightComponent**> dirLights = allLights.Where([](const LightComponent* light) { return light->GetType() == LightType::Directional; });
        Array<LightComponent**> ambLights = allLights.Where([](const LightComponent* light) { return light->GetType() == LightType::Ambient; });

        LightComponent** dirLight = dirLights.IsEmpty() ? nullptr : dirLights.First();
        LightComponent** ambLight = ambLights.IsEmpty() ? nullptr : ambLights.First();

        const Color& dirLightColor = dirLight ? (*dirLight)->GetColor() : Color::Black;
        const float dirLightIntensity = dirLight ? (*dirLight)->GetIntensity() : 0.0f;
        const Vector3 dirLightDir = dirLight ? (*dirLight)->GetTransform()->GetForwardVector() : Vector3::Zero;
        const Color& ambLightColor = ambLight ? (*ambLight)->GetColor() : Color::Black;
        const float ambLightIntensity = ambLight ? (*ambLight)->GetIntensity() : 0.0f;

        const auto ToVector3 = [](const Color& color) -> Vector3
        {
            return Vector3(color.r, color.g, color.b);
        };


        SceneData sceneData;
        sceneData.modelMatrix = modelMatrix;
        sceneData.viewMatrix = viewMatrix;
        sceneData.projectionMatrix = projectionMatrix;
        sceneData.normalMatrix = Matrix3x4(normalMatrix);
        sceneData.cameraPosition = cameraPosition;
        sceneData.directionalLightColor = ToVector3(dirLightColor);
        sceneData.directionalLightIntensity = dirLightIntensity;
        sceneData.directionalLightDirection = dirLightDir;
        sceneData.ambientLightColor = ToVector3(ambLightColor);
        sceneData.ambientLightIntensity = ambLightIntensity;

        cmdBuffer.UpdateBuffer(*m_SceneUniformBuffer, 0, sizeof(SceneData), &sceneData);
    }

    void StaticMeshRenderer::OnRender(CommandBuffer& cmdBuffer)
    {
        Component::OnRender(cmdBuffer);
        if (!m_StaticMesh)
            return;

        if (m_StaticMesh->GetMaterialInfos().IsEmpty())
            return;

        Ref<Buffer> vertexBuffer = m_StaticMesh->GetVertexBuffer();
        if (!vertexBuffer) return;

        Ref<Buffer> indexBuffer = m_StaticMesh->GetIndexBuffer();
        if (!indexBuffer) return;

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
        cmdBuffer.SetViewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
        cmdBuffer.SetScissor(0, 0, (int32_t)width, (int32_t)height);

        const auto& materialInfos = m_StaticMesh->GetMaterialInfos();
        for (const MaterialInfo& materialInfo : materialInfos)
        {
            Ref<Material> material = m_StaticMesh->GetMaterial(materialInfo.slot);
            if (!material) continue;
            cmdBuffer.BindMaterial(*material);

            for (const SubMeshInfo& subMesh : materialInfo.subMeshes)
            {
                cmdBuffer.BindVertexBuffer(*vertexBuffer, subMesh.vertexBufferOffset);
                cmdBuffer.BindIndexBuffer(*indexBuffer, subMesh.indexBufferOffset, Format::Uint32);
                cmdBuffer.DrawIndexed(subMesh.indexBufferSize / sizeof(uint32_t), 0);
            }
        }
    }

    Ref<StaticMesh> StaticMeshRenderer::GetStaticMesh() const
    {
        return m_StaticMesh;
    }

    void StaticMeshRenderer::SetStaticMesh(const Ref<StaticMesh>& newMesh)
    {
        const Application* application = GetApplication();
        const Ref<Device>& device = application->GetDevice();
        device->WaitIdle();

        m_StaticMesh = newMesh;
    }
}
