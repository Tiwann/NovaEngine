#include "HelloTexture.h"
#include "Runtime/EntryPoint.h"
#include "CommandLine/ArgumentParser.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanTexture2D.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Pipeline.h"
#include "Rendering/Shader.h"
#include "Rendering/Vertex.h"
#include "Rendering/VertexBuffer.h"
#include "ResourceManager/ShaderManager.h"
#include "ResourceManager/TextureManager.h"
#include "Runtime/Window.h"
#include "Runtime/Log.h"


NOVA_DEFINE_APPLICATION_CLASS(HelloTexture)

namespace Nova
{
    HelloTexture::HelloTexture(const Array<const char*>& Arguments) : Application(Arguments)
    {

    }

    ApplicationConfiguration HelloTexture::CreateConfiguration() const
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

    void HelloTexture::OnInit()
    {
        Application::OnInit();

        const Path ShaderPath = Path(NOVA_APPLICATION_ROOT_DIR) / "Assets/Shaders/HelloTexture.slang";
        if (m_Shader = m_ShaderManager->Load("HelloTexture", ShaderPath); !m_Shader)
        {
            RequireExit(ExitCode::Error);
            return;
        }

        const Path TexturePath = Path(NOVA_APPLICATION_ROOT_DIR) / "Assets/Textures/Checker.png";
        if (m_Texture = m_TextureManager->Load("Texture", TexturePath); !m_Texture)
        {
            RequireExit(ExitCode::Error);
            return;
        }


        const i32 Width = GetWindow()->GetWidth<i32>();
        const i32 Height = GetWindow()->GetHeight<i32>();
        const f32 AspectRatio = (f32)Width / (f32)Height;

        const Array<Vertex> Vertices
        {
            Vertex({ -0.5f / AspectRatio, -0.5f, 0.0f }, { 0.0f, 0.0f }, Vector3::Zero, Color::Red),
            Vertex({ -0.5f / AspectRatio, +0.5f, 0.0f }, { 0.0f, 1.0f }, Vector3::Zero, Color::Green),
            Vertex({ +0.5f / AspectRatio, +0.5f, 0.0f }, { 1.0f, 1.0f }, Vector3::Zero, Color::Blue),
            Vertex({ +0.5f / AspectRatio, -0.5f, 0.0f }, { 1.0f, 0.0f }, Vector3::Zero, Color::White),
        };

        const Array<u32> Indices { 0, 1, 2, 0, 2, 3 };

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


        const VkDescriptorImageInfo ImageInfo
        {
            .sampler = m_Texture->As<VulkanTexture2D>()->GetSampler(),
            .imageView = m_Texture->As<VulkanTexture2D>()->GetImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        const VkDevice Device = GetRenderer()->As<VulkanRenderer>()->GetDevice();
        const auto& Sets = m_Shader->As<VulkanShader>()->GetDescriptorSets();
        VkWriteDescriptorSet WriteDescriptors[1] {  };
        WriteDescriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        WriteDescriptors[0].descriptorCount = 1;
        WriteDescriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        WriteDescriptors[0].dstSet = Sets[0];
        WriteDescriptors[0].dstBinding = 1;
        WriteDescriptors[0].pImageInfo = &ImageInfo;


        vkUpdateDescriptorSets(Device, 1, WriteDescriptors, 0, nullptr);
    }

    void HelloTexture::OnExit()
    {
        delete m_VertexBuffer;
        delete m_IndexBuffer;

        m_Pipeline->Destroy();
        delete m_Pipeline;
        Application::OnExit();
    }

    void HelloTexture::OnUpdate(const float DeltaTime)
    {
        Application::OnUpdate(DeltaTime);
        static f32 Timer = 0.0f;
        Timer += DeltaTime;
        if (Timer > 1.0f)
        {
            Timer = 0.0f;
            NOVA_LOG(HelloTexture, Verbosity::Info, "FPS: {}", (i32)(1.0f / DeltaTime));
        }
    }

    void HelloTexture::OnFrameStarted(Renderer* Renderer)
    {
	    Application::OnFrameStarted(Renderer);
    }

    void HelloTexture::OnRender(Renderer* Renderer)
    {
        Application::OnRender(Renderer);
        Renderer->BindPipeline(m_Pipeline);
        Renderer->DrawIndexed(m_VertexBuffer, m_IndexBuffer);
    }
}
