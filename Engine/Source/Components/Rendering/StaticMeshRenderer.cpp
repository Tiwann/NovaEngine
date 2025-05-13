#include "StaticMeshRenderer.h"

#include "DirectionalLight.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Runtime/StaticMesh.h"
#include "Runtime/PopupMessage.h"
#include "Editor/EditorGUI.h"
#include "Rendering/Pipeline.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/Application.h"
#include "Rendering/Renderer.h"
#include "Rendering/VertexBuffer.h"
#include "Runtime/Scene.h"
#include "Runtime/Window.h"

#include <vulkan/vulkan.h>

#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"

namespace Nova
{
    StaticMeshRenderer::StaticMeshRenderer(Entity* Owner) : Component(Owner, "Model Component")
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

        PipelineSpecification PipelineSpecification;
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
        PipelineSpecification.RasterizationSamples = 1;
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
        m_Pipeline = Renderer->CreatePipeline(PipelineSpecification);

        m_SceneUniformBuffer = Renderer->CreateUniformBuffer(2 * sizeof(Matrix4));


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

            Array<VkWriteDescriptorSet> WriteDescriptors { SceneWriteDescriptor };
            vkUpdateDescriptorSets(Device, WriteDescriptors.Count(), WriteDescriptors.Data(), 0, nullptr);
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

        const Matrix4& ViewProjection = Camera->GetViewProjectionMatrix();

        const Entity* Owner = GetOwner();
        Transform* EntityTransform = Owner->GetTransform();
        const Matrix4& Model = EntityTransform->GetWorldSpaceMatrix();

        const struct SceneData
        {
            Matrix4 ViewProjectionMatrix;
            Matrix4 ModelMatrix;
        } SceneDataInstance
        {
            .ViewProjectionMatrix = ViewProjection,
            .ModelMatrix = Model
        };

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

        const Transform* EntityTransform = GetTransform();
        const Transform* CameraTransform = Camera->GetTransform();
        const Vector3& CameraPosition = CameraTransform->GetPosition();
        const Vector3& EntityPosition = EntityTransform->GetPosition();
        const Vector3& EntityRotation = EntityTransform->GetRotation();
        const Vector3 CameraViewDirection = Math::Normalize(CameraPosition - EntityPosition);
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
