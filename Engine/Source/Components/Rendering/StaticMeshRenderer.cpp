#include "StaticMeshRenderer.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Runtime/StaticMesh.h"
#include "Runtime/PopupMessage.h"
#include "Editor/EditorGUI.h"
#include "Rendering/Pipeline.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/Application.h"
#include "Rendering/Renderer.h"
#include "Runtime/Scene.h"
#include "Runtime/Window.h"

#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"


#include "Math/Matrix3x4.h"
#include "Math/Vector3.h"

namespace Nova
{
    struct SceneData
    {
        Matrix4 ModelMatrix;
        Matrix4 ViewMatrix;
        Matrix4 ProjectionMatrix;
        Matrix3x4 NormalMatrix;
        Vector3 CameraViewDirection;
        float DirectionalLightIntensity;
        Vector3 DirectionalLightColor;
        float Padding2;
        Vector3 DirectionalLightDirection;
        float AmbientLightIntensity;
        Vector3 AmbientLightColor;
    };

    StaticMeshRenderer::StaticMeshRenderer(Entity* Owner) : Component(Owner, "Static Mesh Component")
    {
    }

    void StaticMeshRenderer::OnInit()
    {
        Component::OnInit();
        m_StaticMesh = new StaticMesh("TransientModel");

        const Entity* Owner = GetOwner();
        const Scene* Scene = Owner->GetScene();
        const Application* Application = Scene->GetOwner();
        const Window* Window = Application->GetWindow();
        const f32 Width = Window->GetWidth();
        const f32 Height = Window->GetHeight();
        Renderer* Renderer = Application->GetRenderer();

        ShaderManager* ShaderManager = g_Application->GetShaderManager();
        Shader* BlinnPhongShader = ShaderManager->Retrieve("HelloCube");

        PipelineCreateInfo PipelineSpecification;
        PipelineSpecification.VertexLayout.AddAttribute({"POSITION", Format::Vector3});
        PipelineSpecification.VertexLayout.AddAttribute({"TEXCOORDINATE", Format::Vector2});
        PipelineSpecification.VertexLayout.AddAttribute({"NORMAL", Format::Vector3});
        PipelineSpecification.VertexLayout.AddAttribute({"COLOR", Format::Vector4});
        PipelineSpecification.BlendEnable = false;
        PipelineSpecification.CullMode = CullMode::BackFace;
        PipelineSpecification.FrontFace = FrontFace::CounterClockwise;
        PipelineSpecification.Viewport = Viewport(0.0f, 0.0f, Width, Height, 0.0f, 1.0f);
        PipelineSpecification.Scissor = Scissor(0, 0, (i32)Width, (i32)Height);
        PipelineSpecification.PolygonMode = PolygonMode::Fill;
        PipelineSpecification.PrimitiveTopology = PrimitiveTopology::TriangleList;
        PipelineSpecification.DepthBiasEnable = false;
        PipelineSpecification.DepthClampEnable = false;
        PipelineSpecification.DepthTestEnable = true;
        PipelineSpecification.DepthWriteEnable = true;
        PipelineSpecification.DepthCompareOperation = CompareOperation::Less;
        PipelineSpecification.PrimitiveRestartEnable = false;
        PipelineSpecification.RasterizerDiscardEnable = false;
        PipelineSpecification.StencilTestEnable = false;
        PipelineSpecification.DynamicRendering = false;
        PipelineSpecification.ShaderProgram = BlinnPhongShader;
        PipelineSpecification.RasterizationSamples = SampleCount::S8;
        PipelineSpecification.MultisampleEnable = true;

        m_Pipeline = Renderer->CreatePipeline(PipelineSpecification);

        m_SceneUniformBuffer = Renderer->CreateUniformBuffer(sizeof(SceneData));


        // NEED TO ABSTRACT THIS
        if (Renderer->GetGraphicsApi() == GraphicsApi::Vulkan)
        {
            const VkDevice Device = Renderer->As<VulkanRenderer>()->GetDevice();
            const auto& Sets = BlinnPhongShader->As<VulkanShader>()->GetDescriptorSets();


            VkDescriptorBufferInfo BufferInfo { };
            BufferInfo.buffer = m_SceneUniformBuffer->As<VulkanUniformBuffer>()->GetHandle();
            BufferInfo.offset = 0;
            BufferInfo.range = VK_WHOLE_SIZE;

            VkWriteDescriptorSet SceneWriteDescriptor { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            SceneWriteDescriptor.descriptorCount = 1;
            SceneWriteDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            SceneWriteDescriptor.dstSet = Sets[0];
            SceneWriteDescriptor.dstBinding = 0;
            SceneWriteDescriptor.pBufferInfo = &BufferInfo;

            vkUpdateDescriptorSets(Device, 1, &SceneWriteDescriptor, 0, nullptr);
        }
    }

    void StaticMeshRenderer::OnDestroy()
    {
        Component::OnDestroy();
        delete m_StaticMesh;

        m_SceneUniformBuffer->Free();
        delete m_SceneUniformBuffer;

        m_Pipeline->Destroy();
        delete m_Pipeline;
    }

    void StaticMeshRenderer::OnFrameBegin(Renderer* Renderer)
    {
        Component::OnFrameBegin(Renderer);
        Camera* Camera = Renderer->GetCurrentCamera();
        if (!Camera) return;

        Entity* Owner = GetOwner();
        Scene* Scene = Owner->GetScene();

        Transform* EntityTransform = Owner->GetTransform();
        Transform* CameraTransform = Camera->GetTransform();

        const Matrix4& ModelMatrix = EntityTransform->GetWorldSpaceMatrix();
        const Matrix4& ViewMatrix = Camera->GetViewMatrix();
        const Matrix4& ProjectionMatrix = Camera->GetProjectionMatrix();
        const Matrix3& NormalMatrix = EntityTransform->GetWorldSpaceNormalMatrix();

        const Vector3& CameraPosition = CameraTransform->GetPosition();
        const Vector3& EntityPosition = EntityTransform->GetPosition();
        const Vector3 CameraViewDirection = Math::Normalize(CameraPosition - EntityPosition);

        const Array<LightComponent*> AllLights = Scene->GetAllComponents<LightComponent>();
        LightComponent** DirectionalLight = AllLights.Where([](LightComponent* Light) { return Light->GetType() == LightType::Directional; }).First();
        LightComponent** AmbientLight = AllLights.Where([](LightComponent* Light) { return Light->GetType() == LightType::Ambient; }).First();

        const Color& DirectionalLightColor = DirectionalLight ? (*DirectionalLight)->GetColor() : Color::Black;
        const float DirectionalLightIntensity = DirectionalLight ? (*DirectionalLight)->GetIntensity() : 0.0f;
        const Vector3 DirectionalLightDirection = DirectionalLight ? (*DirectionalLight)->GetTransform()->GetForwardVector() : Vector3::Zero;
        const Color& AmbientLightColor = AmbientLight ? (*AmbientLight)->GetColor() : Color::Black;
        const float AmbientLightIntensity = AmbientLight ? (*AmbientLight)->GetIntensity() : 0.0f;

        const auto ToVector3 = [](const Color& Color) -> Vector3
        {
            return Vector3(Color.r, Color.g, Color.b);
        };

        SceneData SceneDataInstance;
        SceneDataInstance.ModelMatrix = ModelMatrix;
        SceneDataInstance.ViewMatrix = ViewMatrix;
        SceneDataInstance.ProjectionMatrix = ProjectionMatrix;
        SceneDataInstance.NormalMatrix = Matrix3x4(NormalMatrix);
        SceneDataInstance.CameraViewDirection = CameraViewDirection;
        SceneDataInstance.DirectionalLightColor = ToVector3(DirectionalLightColor);
        SceneDataInstance.DirectionalLightIntensity = DirectionalLightIntensity;
        SceneDataInstance.DirectionalLightDirection = DirectionalLightDirection;
        SceneDataInstance.AmbientLightColor = ToVector3(AmbientLightColor);
        SceneDataInstance.AmbientLightIntensity = AmbientLightIntensity;

        Renderer->UpdateUniformBuffer(m_SceneUniformBuffer, 0, sizeof(SceneData), &SceneDataInstance);
    }

    void StaticMeshRenderer::OnRender(Renderer* Renderer)
    {
        Component::OnRender(Renderer);
        if (!m_StaticMesh)
            return;

        if (m_StaticMesh->GetSubMeshes().IsEmpty())
            return;

        // THIS NEEDS TO BE OPTIMIZED BY FILTER THE AVAILABLE LIGHTS
        Entity* Owner = GetOwner();
        Scene* Scene = Owner->GetScene();
        const Application* Application = Scene->GetOwner();
        const Window* Window = Application->GetWindow();

        const Array<LightComponent*> AllLights = Scene->GetAllComponents<LightComponent>();
        
        const Camera* Camera = Renderer->GetCurrentCamera();
        if (!Camera) return;

        const f32 Width = Window->GetWidth();
        const f32 Height = Window->GetHeight();

        Renderer->BindPipeline(m_Pipeline);
        Renderer->SetViewport(Viewport(0.0f, 0.0f, Width, Height, 0.0f, 1.0f));
        Renderer->SetScissor(Scissor(0, 0, (i32)Width, (i32)Height));

        for (const SubMesh& SubMesh : m_StaticMesh->GetSubMeshes())
        {
            Renderer->BindVertexBuffer(m_StaticMesh->GetVertexBuffer(), SubMesh.VertexBufferOffset);
            Renderer->BindIndexBuffer(m_StaticMesh->GetIndexBuffer(), SubMesh.IndexBufferOffset);
            Renderer->DrawIndexed(SubMesh.IndexBufferSize / sizeof(u32), 0);
        }

        /*u32 PassIndex = 0;
        for (const LightComponent* Light : AllLights)
        {
            if (!Light->IsEnabled()) continue;

            const f32 Width = Window->GetWidth<f32>();
            const f32 Height = Window->GetHeight<f32>();
            Renderer->BindPipeline(m_Pipeline);
            Renderer->SetViewport(Viewport(0.0f, 0.0f, Width, Height, 0.0f, 1.0f));
            Renderer->SetScissor(Scissor(0, 0, (i32)Width, (i32)Height));

            for (const SubMesh& SubMesh : m_StaticMesh->GetSubMeshes())
            {
                Renderer->BindVertexBuffer(m_StaticMesh->GetVertexBuffer(), SubMesh.VertexBufferOffset);
                Renderer->BindIndexBuffer(m_StaticMesh->GetIndexBuffer(), SubMesh.IndexBufferOffset);
                Renderer->DrawIndexed(SubMesh.IndexBufferSize / sizeof(u32));
            }
            PassIndex++;
        }*/
    }

    void StaticMeshRenderer::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);
        if (UI::Button("Load Model"))
        {
            OpenFile();
        }
    }

    void StaticMeshRenderer::OnModelChange(StaticMesh* NewModel)
    {
        
    }

    bool StaticMeshRenderer::OpenFile()
    {
        const Path Filepath = File::OpenFileDialog("Open 3D mode file...", "", DialogFilters::ModelFilters);
        PopupMessage* Message = PopupMessage::Create("Warning", "Failed to open 3D model", PopupMessageResponse::OK, PopupMessageIcon::Warning);
            
        if (!File::Exists(Filepath))
        {
            if (Message) Message->Show();
            delete Message;
            return false;
        }
            
        StaticMesh* model = new StaticMesh("Transient Static Mesh");
        if (!model->LoadFromFile(Filepath))
        {
            if (Message) Message->Show();
            delete model;
            delete Message;
            return false;
        }

        if (m_StaticMesh)
        {
            delete m_StaticMesh;
            m_StaticMesh = model;
            OnModelChange(m_StaticMesh);
        }

        delete Message;
        return true;
    }

    StaticMesh* StaticMeshRenderer::GetStaticMesh() const
    {
        return m_StaticMesh;
    }

    void StaticMeshRenderer::SetStaticMesh(StaticMesh* NewStaticMesh)
    {
        m_StaticMesh = NewStaticMesh;
    }
}
