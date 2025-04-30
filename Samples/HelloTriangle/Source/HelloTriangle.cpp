#include "HelloTriangle.h"
#include "CommandLine/ArgumentParser.h"
#include "Platform/Vulkan/VulkanTexture2D.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Pipeline.h"
#include "Rendering/Shader.h"
#include "Rendering/Vertex.h"
#include "Rendering/VertexBuffer.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/EntryPoint.h"
#include "Runtime/Log.h"
#include "Runtime/Window.h"


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
            Vertex({ -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }, Vector3::Zero, Color::Red),
            Vertex({ +0.0f, +0.5f, 0.0f }, { 0.0f, 1.0f }, Vector3::Zero, Color::Green),
            Vertex({ +0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, Vector3::Zero, Color::Blue),
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
    }

    void HelloTriangle::OnExit()
    {
        delete m_VertexBuffer;
        delete m_IndexBuffer;

        m_Pipeline->Destroy();
        delete m_Pipeline;
        Application::OnExit();
    }

    void HelloTriangle::OnUpdate(const float DeltaTime)
    {
        Application::OnUpdate(DeltaTime);
        static f32 Timer = 0.0f;
        Timer += DeltaTime;
        if (Timer > 1.0f)
        {
            Timer = 0.0f;
            NOVA_LOG(HelloTriangle, Verbosity::Info, "FPS: {}", (i32)(1.0f / DeltaTime));
        }
    }

    void HelloTriangle::OnFrameStarted(Renderer* Renderer)
    {
	    Application::OnFrameStarted(Renderer);
    }

    void HelloTriangle::OnRender(Renderer* Renderer)
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
