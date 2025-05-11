#include "HelloCube.h"
#include "CommandLine/ArgumentParser.h"
#include "Platform/Vulkan/VulkanTexture2D.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Pipeline.h"
#include "Rendering/Shader.h"
#include "Rendering/Vertex.h"
#include "Rendering/VertexBuffer.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/EntityHandle.h"
#include "Runtime/EntryPoint.h"
#include "Runtime/Log.h"
#include "Runtime/Scene.h"
#include "Runtime/Window.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Platform/Vulkan/VulkanCommandBuffer.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"

NOVA_DEFINE_APPLICATION_CLASS(HelloCube)


namespace Nova
{
    HelloCube::HelloCube(const Array<const char*>& Arguments) : Application(Arguments)
    {

    }

    ApplicationConfiguration HelloCube::CreateConfiguration() const
    {
        ApplicationConfiguration Configuration;
        Configuration.AppName = "Hello Cube | Nova Engine";
        Configuration.WindowWidth = 600;
        Configuration.WindowHeight = 400;
        Configuration.WindowResizable = true;
        Configuration.Audio.SampleRate = 44100;
        Configuration.Audio.BufferSize = 1024;
        Configuration.Audio.BufferCount = 4;
        Configuration.Graphics.GraphicsApi = GraphicsApi::Vulkan;
        Configuration.Graphics.BufferType = SwapchainBuffering::TripleBuffering;
        Configuration.Graphics.VSync = true;
        Configuration.WithEditor = true;
        return Configuration;
    }

    struct MeshData
    {
        size_t VertexBufferOffset = 0;
        size_t VertexBufferSize = 0;
        size_t IndexBufferOffset = 0;
        size_t IndexBufferSize = 0;
    };

    static Array<MeshData> Meshes;
    static EntityHandle Entity = nullptr;

    void HelloCube::OnInit()
    {
        Application::OnInit();

        const Path ShaderPath = Path(NOVA_APPLICATION_ROOT_DIR) / "Assets/Shaders/HelloCube.slang";
        if (m_Shader = m_ShaderManager->Load("HelloCube", ShaderPath); !m_Shader)
        {
            RequireExit(ExitCode::Error);
            throw;
        }

        constexpr u32 Flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices;
        DialogFilters Filters = DialogFilters::ModelFilters;
        const Path ModelFilepath = File::OpenFileDialog("Please select a 3D model file", "", DialogFilters::ModelFilters);
        Assimp::Importer ModelImporter;
        const aiScene* LoadedScene = ModelImporter.ReadFile(ModelFilepath.string().c_str(), Flags);
        if (!LoadedScene)
        {
            NOVA_LOG(HelloCube, Verbosity::Error, "Failed to load model file!");
            RequireExit(ExitCode::Error);
            throw;
        }

        if (!LoadedScene->HasMeshes())
        {
            NOVA_LOG(HelloCube, Verbosity::Error, "Model file doesn't contain any Meshes!");
            RequireExit(ExitCode::Error);
            throw;
        }

        m_CameraEntity = GetScene()->CreateEntity("Camera");
        m_Camera = m_CameraEntity->AddComponent<Camera>();
        m_Camera->Settings = CameraSettings::DefaultPerspective.WithFOV(45.0f);

        Entity = GetScene()->CreateEntity("Entity");


        Array<Vertex> AllVertices;
        Array<u32> AllIndices;

        for (size_t MeshIndex = 0; MeshIndex < LoadedScene->mNumMeshes; MeshIndex++)
        {
            aiMesh* LoadedMesh = LoadedScene->mMeshes[MeshIndex];

            Array<u32> Indices;
            for (u32 FaceIndex = 0; FaceIndex < LoadedMesh->mNumFaces; ++FaceIndex)
            {
                const aiFace& Face = LoadedMesh->mFaces[FaceIndex];
                for (u32 i = 0; i < Face.mNumIndices; ++i)
                    Indices.Add(Face.mIndices[i]);
            }

            Array<Vertex> Vertices;
            for (u32 VertexIndex = 0; VertexIndex < LoadedMesh->mNumVertices; ++VertexIndex)
            {
                const aiVector3D& Position = LoadedMesh->HasPositions() ? LoadedMesh->mVertices[VertexIndex] : aiVector3D(0, 0, 0);
                const aiVector3D& TexCoord = LoadedMesh->HasTextureCoords(0) ? LoadedMesh->mTextureCoords[0][VertexIndex] : aiVector3D(0, 0, 0);
                const aiVector3D& Normal = LoadedMesh->HasNormals() ? LoadedMesh->mNormals[VertexIndex] : aiVector3D(0, 0, 0);
                const aiColor4D& Color = LoadedMesh->HasVertexColors(0) ? LoadedMesh->mColors[0][VertexIndex] : aiColor4D(0, 0, 0, 0);

                Vector3(* const ToVector3)(const aiVector3D&) = [](const aiVector3D& In) { return Vector3(In.x, In.y, In.z); };
                Vector2(* const ToVector2)(const aiVector3D&) = [](const aiVector3D& In) { return Vector2(In.x, In.y); };
                Vector4(* const ToVector4)(const aiColor4D&) = [](const aiColor4D& In) { return Vector4(In.r, In.g, In.b, In.a); };


                const Vertex Vertex {
                    .Position = ToVector3(Position),
                    .TextureCoordinate = ToVector2(TexCoord),
                    .Normal = ToVector3(Normal),
                    .Color = ToVector4(Color)
                };
                Vertices.Add(Vertex);
            }

            AllVertices.AddRange(Vertices);
            AllIndices.AddRange(Indices);

            MeshData Mesh { };
            Mesh.VertexBufferSize = Vertices.Count() * sizeof(Vertex);
            Mesh.VertexBufferOffset = MeshIndex == 0 ? 0 : Meshes[MeshIndex - 1].VertexBufferOffset + Meshes[MeshIndex - 1].VertexBufferSize;
            Mesh.IndexBufferOffset = Indices.Count() * sizeof(u32);
            Mesh.IndexBufferSize = MeshIndex == 0 ? 0 : Meshes[MeshIndex - 1].IndexBufferOffset + Meshes[MeshIndex - 1].IndexBufferSize;
            Meshes.Add(Mesh);
        }

        Renderer* Renderer = GetRenderer();
        m_VertexBuffer = Renderer->CreateVertexBuffer(BufferView(AllVertices.Data(), AllVertices.Count()));
        m_IndexBuffer = Renderer->CreateIndexBuffer(BufferView(AllIndices.Data(), AllIndices.Count()));

        PipelineSpecification PipelineSpecification;
        PipelineSpecification.VertexLayout.AddAttribute({"POSITION", Format::Vector3});
        PipelineSpecification.VertexLayout.AddAttribute({"TEXCOORDINATE", Format::Vector2});
        PipelineSpecification.VertexLayout.AddAttribute({"NORMAL", Format::Vector3});
        PipelineSpecification.VertexLayout.AddAttribute({"COLOR", Format::Vector4});
        PipelineSpecification.BlendEnable = false;
        PipelineSpecification.CullMode = CullMode::BackFace;
        PipelineSpecification.FrontFace = FrontFace::CounterClockwise;
        PipelineSpecification.Viewport = Viewport(0.0f, 0.0f, 600.0f, 400.0f, 0.0f, 1.0f);
        PipelineSpecification.Scissor = Scissor(0, 0, 600, 400);
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
        PipelineSpecification.ShaderProgram = m_Shader;
        m_Pipeline = Renderer->CreatePipeline(PipelineSpecification);

        m_CameraUniformBuffer = new VulkanUniformBuffer(Renderer);
        m_CameraUniformBuffer->Allocate(2 * sizeof(Matrix4));

        m_EntityUniformBuffer = new VulkanUniformBuffer(Renderer);
        m_EntityUniformBuffer->Allocate(sizeof(Matrix4));

        const VkDevice Device = GetRenderer()->As<VulkanRenderer>()->GetDevice();
        const auto& Sets = m_Shader->As<VulkanShader>()->GetDescriptorSets();


        VkDescriptorBufferInfo BufferInfos[2] { };
        BufferInfos[0].buffer = m_CameraUniformBuffer->As<VulkanUniformBuffer>()->GetHandle();
        BufferInfos[0].offset = 0;
        BufferInfos[0].range = VK_WHOLE_SIZE;

        BufferInfos[1].buffer = m_EntityUniformBuffer->As<VulkanUniformBuffer>()->GetHandle();
        BufferInfos[1].offset = 0;
        BufferInfos[1].range = VK_WHOLE_SIZE;



        VkWriteDescriptorSet CameraWriteDescriptors { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        CameraWriteDescriptors.descriptorCount = 1;
        CameraWriteDescriptors.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        CameraWriteDescriptors.dstSet = Sets[0];
        CameraWriteDescriptors.dstBinding = 0;
        CameraWriteDescriptors.pBufferInfo = &BufferInfos[0];

        VkWriteDescriptorSet ModelWriteDescriptors { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        ModelWriteDescriptors.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        ModelWriteDescriptors.descriptorCount = 1;
        ModelWriteDescriptors.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ModelWriteDescriptors.dstSet = Sets[0];
        ModelWriteDescriptors.dstBinding = 1;
        ModelWriteDescriptors.pBufferInfo = &BufferInfos[1];

        Array<VkWriteDescriptorSet> WriteDescriptors { ModelWriteDescriptors, CameraWriteDescriptors };

        vkUpdateDescriptorSets(Device, WriteDescriptors.Count(), WriteDescriptors.Data(), 0, nullptr);
    }

    void HelloCube::OnExit()
    {
        delete m_VertexBuffer;
        delete m_IndexBuffer;

        m_CameraUniformBuffer->Free();
        delete m_CameraUniformBuffer;
        m_EntityUniformBuffer->Free();
        delete m_EntityUniformBuffer;

        m_Pipeline->Destroy();
        delete m_Pipeline;
        Application::OnExit();
    }

    void HelloCube::OnUpdate(const float DeltaTime)
    {
        Application::OnUpdate(DeltaTime);
    }

    void HelloCube::OnFrameStarted(Renderer* Renderer)
    {
	    Application::OnFrameStarted(Renderer);

        const Matrix4& View = m_Camera->GetViewMatrix();
        const Matrix4& Projection = m_Camera->GetProjectionMatrix();
        const Matrix4& Model = Entity->GetTransform()->GetWorldSpaceMatrix();

        Renderer->UpdateUniformBuffer(m_CameraUniformBuffer, 0, sizeof(Matrix4), &View);
        Renderer->UpdateUniformBuffer(m_CameraUniformBuffer, sizeof(Matrix4), sizeof(Matrix4), &Projection);
        Renderer->UpdateUniformBuffer(m_EntityUniformBuffer, 0, sizeof(Matrix4), &Model);
    }

    void HelloCube::OnRender(Renderer* Renderer)
    {
        Application::OnRender(Renderer);
        const f32 Width = GetWindow()->GetWidth<f32>();
        const f32 Height = GetWindow()->GetHeight<f32>();

        Renderer->BindPipeline(m_Pipeline);
        Renderer->SetViewport(Viewport(0.0f, 0.0f, Width, Height, 0.0f, 1.0f));
        Renderer->SetScissor(Scissor(0, 0, (int)Width, (int)Height));
        Renderer->DrawIndexed(m_VertexBuffer, m_IndexBuffer);
    }
}
