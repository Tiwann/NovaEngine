#include "StaticMeshRenderer.h"
#include "LightComponent.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Runtime/StaticMesh.h"
#include "Rendering/GraphicsPipeline.h"
#include "Runtime/AssetDatabase.h"
#include "Rendering/Shader.h"
#include "Runtime/Application.h"
#include "Rendering/RenderDevice.h"
#include "Runtime/Scene.h"
#include "Runtime/Window.h"
#include "Math/Matrix3x4.h"
#include "Math/Vector3.h"
#include "Rendering/Buffer.h"
#include "Rendering/CommandBuffer.h"
#include "Rendering/ShaderBindingSet.h"

namespace Nova
{
    struct alignas(16) CameraData
    {
        Matrix4 viewMatrix;
        Matrix4 inverseViewMatrix;
        Matrix4 projectionMatrix;
        Matrix4 inverseProjectionMatrix;
        Matrix4 viewProjectionMatrix;
        Matrix4 inverseViewProjectionMatrix;
        Vector4 cameraPos;
        Vector4 cameraDir;
    };

    struct alignas(16) ObjectData
    {
        Matrix4 modelMatrix;
        Matrix4 normalMatrix;
    };

    struct alignas(16) DirectionalLightData
    {
        Vector3 color;
        float intensity;
        Vector3 direction;
        float padding;
    };

    struct alignas(16) AmbientLightData
    {
        Vector3 color;
        float intensity;
    };

    struct alignas(16) SceneData
    {
        DirectionalLightData directionalLight;
        AmbientLightData ambientLight;
        uint32_t pointLightCount;
        Vector3 padding0;
        uint32_t spotLightCount;
        Vector3 padding1;
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
        Ref<RenderDevice> device = application->GetRenderDevice();

        const AssetDatabase& assetDatabase = application->GetAssetDatabase();
        m_Shader = assetDatabase.Get<Shader>("PBRShadingShader");
        VertexLayout vertexLayout;
        vertexLayout.AddInputBinding(0, VertexInputRate::Vertex);
        vertexLayout.AddInputAttribute("POSITION", ShaderDataType::Float3, 0);
        vertexLayout.AddInputAttribute("TEXCOORDINATE", ShaderDataType::Float2, 0);
        vertexLayout.AddInputAttribute("NORMAL", ShaderDataType::Float3, 0);
        vertexLayout.AddInputAttribute("TANGENT", ShaderDataType::Float3, 0);
        vertexLayout.AddInputAttribute("COLOR", ShaderDataType::Float4, 0);

        GraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.device = device;
        pipelineCreateInfo.shader = m_Shader;
        pipelineCreateInfo.multisampleState.sampleCount = 8;
        pipelineCreateInfo.depthStencilState.depthWriteEnable = true;
        pipelineCreateInfo.depthStencilState.depthTestEnable = true;
        pipelineCreateInfo.vertexInputState = CreateInputStateFromVertexLayout(vertexLayout);
        pipelineCreateInfo.colorAttachmentFormats.Add(Format::R8G8B8A8_SRGB);
        pipelineCreateInfo.depthAttachmentFormat = Format::D32_FLOAT_S8_UINT;
        m_Pipeline = device->CreateGraphicsPipeline(pipelineCreateInfo);

        BufferCreateInfo uniformBufferCreateInfo;
        uniformBufferCreateInfo.device = device;
        uniformBufferCreateInfo.size = sizeof(SceneData);
        uniformBufferCreateInfo.usage = BufferUsage::UniformBuffer;
        m_SceneUniformBuffer = device->CreateBuffer(uniformBufferCreateInfo);

        uniformBufferCreateInfo.size = sizeof(ObjectData);
        m_ObjectUniformBuffer = device->CreateBuffer(uniformBufferCreateInfo);

        uniformBufferCreateInfo.size = sizeof(CameraData);
        m_CameraUniformBuffer = device->CreateBuffer(uniformBufferCreateInfo);

        const SamplerCreateInfo samplerCreateInfo = SamplerCreateInfo()
        .WithAddressMode(SamplerAddressMode::Repeat)
        .WithFilter(Filter::Linear, Filter::Linear)
        .WithLODRange(0.0f, 1.0f);
        m_Sampler = device->GetOrCreateSampler(samplerCreateInfo);

        m_BindingSet1 = m_Shader->CreateBindingSet(1);
        m_BindingSet2 = m_Shader->CreateBindingSet(2);

        m_BindingSet1->BindBuffer(0, *m_ObjectUniformBuffer, 0, ~0);
        m_BindingSet1->BindSampler(1, *m_Sampler);
        m_BindingSet2->BindBuffer(0, *m_CameraUniformBuffer, 0, ~0);
        m_BindingSet2->BindBuffer(1, *m_SceneUniformBuffer, 0, ~0);
    }

    void StaticMeshRenderer::OnDestroy()
    {
        Component::OnDestroy();
        const Entity* owner = GetOwner();
        const Scene* scene = owner->GetOwner();
        Application* application = scene->GetOwner();
        Ref<RenderDevice> device = application->GetRenderDevice();
        device->WaitIdle();

        m_BindingSet1->Destroy();
        m_BindingSet2->Destroy();

        m_SceneUniformBuffer->Destroy();
        m_CameraUniformBuffer->Destroy();
        m_ObjectUniformBuffer->Destroy();
        m_Pipeline->Destroy();
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
        const Vector3& cameraDirection = cameraTransform->GetForwardVector();
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
        sceneData.directionalLight = { ToVector3(dirLightColor), dirLightIntensity, dirLightDir };
        sceneData.ambientLight = { ToVector3(ambLightColor), ambLightIntensity };
        sceneData.pointLightCount = 0;
        sceneData.spotLightCount = 0;
        cmdBuffer.UpdateBuffer(*m_SceneUniformBuffer, 0, sizeof(SceneData), &sceneData);


        ObjectData objectData;
        objectData.modelMatrix = modelMatrix;
        objectData.normalMatrix = Matrix4(
            Vector4(normalMatrix[0], 0.0f),
            Vector4(normalMatrix[1], 0.0f),
            Vector4(normalMatrix[2], 0.0f),
            Vector4(0.0f, 0.0f, 0.0f, 1.0f));
        cmdBuffer.UpdateBuffer(*m_ObjectUniformBuffer, 0, sizeof(ObjectData), &objectData);

        CameraData cameraData;
        cameraData.viewMatrix = viewMatrix;
        cameraData.inverseViewMatrix = viewMatrix.Inverted();
        cameraData.projectionMatrix = projectionMatrix;
        cameraData.inverseProjectionMatrix = projectionMatrix.Inverted();
        cameraData.viewProjectionMatrix = projectionMatrix * viewMatrix;
        cameraData.inverseViewProjectionMatrix = cameraData.viewProjectionMatrix.Inverted();
        cameraData.cameraPos = Vector4(cameraPosition, 0.0f);
        cameraData.cameraDir = Vector4(cameraDirection, 0.0f);
        cmdBuffer.UpdateBuffer(*m_CameraUniformBuffer, 0, sizeof(CameraData), &cameraData);


        struct MaterialParameters
        {
            Vector3 albedo;
            float metallness;
            float roughness;
            Vector3 emissive;
        } materialParameters
        {
            Vector3(1.0f, 1.0f, 1.0f),
            1.0f,
            1.0f,
            Vector3(1.0f, 1.0f, 1.0f)
        };

        cmdBuffer.PushConstants(*m_Shader, ShaderStageFlagBits::Fragment, 0, sizeof(MaterialParameters), &materialParameters);
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
        cmdBuffer.BindShaderBindingSet(*m_Shader, *m_BindingSet1);
        cmdBuffer.BindShaderBindingSet(*m_Shader, *m_BindingSet2);
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
                cmdBuffer.DrawIndexed(subMesh.indexBufferSize / sizeof(uint32_t), 1, 0, 0, 0);
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
        const Ref<RenderDevice>& device = application->GetRenderDevice();
        device->WaitIdle();

        m_StaticMesh = newMesh;
    }
}
