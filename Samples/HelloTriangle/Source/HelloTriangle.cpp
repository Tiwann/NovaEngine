#include "HelloTriangle.h"
#include "Runtime/EntryPoint.h"
#include "CommandLine/ArgumentParser.h"
#include "Platform/Vulkan/DescriptorSetInfo.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Pipeline.h"
#include "Rendering/Shader.h"
#include "Rendering/UniformBuffer.h"
#include "Rendering/Vertex.h"
#include "Rendering/VertexBuffer.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/DynamicLibrary.h"
#include "Runtime/SharedPointer.h"

NOVA_DEFINE_APPLICATION_CLASS(HelloTriangle)

namespace Nova
{
    HelloTriangle::HelloTriangle(const Array<const char*>& Arguments) : Application(Arguments)
    {

    }

    ApplicationConfiguration HelloTriangle::CreateConfiguration() const
    {
        ApplicationConfiguration Configuration;
        Configuration.AppName = "Hello Triangle | Nova Engine";
        Configuration.WindowWidth = 600;
        Configuration.WindowHeight = 400;
        Configuration.WindowResizable = false;
        Configuration.Audio.SampleRate = 44100;
        Configuration.Audio.BufferSize = 1024;
        Configuration.Audio.BufferCount = 4;
        Configuration.Graphics.GraphicsApi = GraphicsApi::Vulkan;
        Configuration.Graphics.BufferType = SwapchainBuffering::TripleBuffering;
        Configuration.Graphics.VSync = true;
        Configuration.WithEditor = false;
        return Configuration;
    }

    static VulkanUniformBuffer* TimeUBO[3];

    void HelloTriangle::OnInit()
    {
        Application::OnInit();

        const Path ShaderPath = Path(NOVA_APPLICATION_ROOT_DIR) / "Assets/Shaders/HelloTriangle.slang";
        if (m_Shader = m_ShaderManager->Load("HelloTriangle", ShaderPath); !m_Shader)
        {
            RequireExit(ExitCode::Error);
            return;
        }

        const Array<Vertex> Vertices
        {
            Vertex(Vector3(-0.5f, -0.5f, 0.0f), Vector2::Zero, Vector3::Zero, Color::Red),
            Vertex(Vector3(+0.0f, +0.5f, 0.0f), Vector2::Zero, Vector3::Zero, Color::Green),
            Vertex(Vector3(+0.5f, -0.5f, 0.0f), Vector2::Zero, Vector3::Zero, Color::Blue),
        };

        const Array<u32> Indices { 0, 1, 2 };

        Renderer* Renderer = GetRenderer();
        m_VertexBuffer = Renderer->CreateVertexBuffer(BufferView(Vertices.Data(), Vertices.Count()));
        m_IndexBuffer = Renderer->CreateIndexBuffer(BufferView(Indices.Data(), Indices.Count()));

        PipelineSpecification PipelineSpecification;
        PipelineSpecification.VertexLayout.AddAttribute({"POSITION", Format::Vector3});
        PipelineSpecification.VertexLayout.AddAttribute({"TEXCOORDINATE", Format::Vector2});
        PipelineSpecification.VertexLayout.AddAttribute({"NORMAL", Format::Vector3});
        PipelineSpecification.VertexLayout.AddAttribute({"COLOR", Format::Vector4});
        PipelineSpecification.BlendEnable = false;
        PipelineSpecification.CullMode = CullMode::None;
        PipelineSpecification.FrontFace = FrontFace::Clockwise;
        PipelineSpecification.Viewport = Viewport(0.0f, 0.0f, 600.0f, 400.0f, 0.0f, 1.0f);
        PipelineSpecification.Scissor = Scissor(0, 0, 600, 400);
        PipelineSpecification.PolygonMode = PolygonMode::Fill;
        PipelineSpecification.PrimitiveTopology = PrimitiveTopology::TriangleList;
        PipelineSpecification.RasterizationSamples = 1;
        PipelineSpecification.DepthBiasEnable = false;
        PipelineSpecification.DepthClampEnable = false;
        PipelineSpecification.DepthTestEnable = false;
        PipelineSpecification.DepthWriteEnable = false;
        PipelineSpecification.PrimitiveRestartEnable = false;
        PipelineSpecification.RasterizerDiscardEnable = false;
        PipelineSpecification.StencilTestEnable = false;
        PipelineSpecification.DynamicRendering = false;
        PipelineSpecification.ShaderProgram = m_Shader;
        m_Pipeline = Renderer->CreatePipeline(PipelineSpecification);

        for (size_t i = 0; i < GetRenderer()->As<VulkanRenderer>()->GetImageCount(); i++)
        {
            TimeUBO[i] = new VulkanUniformBuffer(Renderer);
            TimeUBO[i]->Allocate(sizeof(float));
        }

    }

    void HelloTriangle::OnExit()
    {
        delete m_VertexBuffer;
        delete m_IndexBuffer;

        for (size_t i = 0; i < GetRenderer()->As<VulkanRenderer>()->GetImageCount(); i++)
        {
            TimeUBO[i]->Free();
            delete TimeUBO[i];
        }


        m_Pipeline->Destroy();
        delete m_Pipeline;
        Application::OnExit();
    }

    void HelloTriangle::OnUpdate(const float DeltaTime)
    {
        Application::OnUpdate(DeltaTime);

        const VkDevice Device = GetRenderer()->As<VulkanRenderer>()->GetDevice();

        const float Time = GetTime();
        const u32 Index = GetRenderer()->As<VulkanRenderer>()->GetCurrentFrameIndex();
        VulkanUniformBuffer* CurrentTimeUBO = TimeUBO[Index];
        CurrentTimeUBO->Copy(&Time, sizeof(float), 0);

        const VkDescriptorBufferInfo BufferInfos
        {
            .buffer = CurrentTimeUBO->GetHandle(),
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        const auto& Sets = m_Shader->As<VulkanShader>()->GetDescriptorSets();
        VkWriteDescriptorSet WriteDescriptors { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        WriteDescriptors.descriptorCount = 1;
        WriteDescriptors.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        WriteDescriptors.dstSet = Sets[0];
        WriteDescriptors.dstBinding = 0;
        WriteDescriptors.pBufferInfo = &BufferInfos;
        vkUpdateDescriptorSets(Device, 1, &WriteDescriptors, 0, nullptr);
    }

    void HelloTriangle::OnRender(Renderer* Renderer)
    {
        Application::OnRender(Renderer);
        Renderer->BindPipeline(m_Pipeline);
        Renderer->DrawIndexed(m_VertexBuffer, m_IndexBuffer);
    }
}
