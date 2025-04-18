#include "HelloTriangle.h"
#include "Runtime/EntryPoint.h"
#include "CommandLine/ArgumentParser.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Pipeline.h"
#include "Rendering/Shader.h"
#include "Rendering/Vertex.h"
#include "Rendering/VertexBuffer.h"
#include "ResourceManager/ShaderManager.h"

#include <vulkan/vulkan.h>

NOVA_DEFINE_APPLICATION_CLASS(HelloTriangle)

namespace Nova
{
    class ModelRenderer;

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
        m_Shader = m_ShaderManager->Load("HelloTriangle", ShaderPath);

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
        PipelineSpecification.VertexBufferLayout.AddAttribute({"POSITION", Format::R32G32B32_FLOAT});
        PipelineSpecification.VertexBufferLayout.AddAttribute({"TEXCOORDINATE", Format::R32G32_FLOAT});
        PipelineSpecification.VertexBufferLayout.AddAttribute({"NORMAL", Format::R32G32B32_FLOAT});
        PipelineSpecification.VertexBufferLayout.AddAttribute({"COLOR", Format::R32G32B32A32_FLOAT});
        PipelineSpecification.BlendEnable = false;
        PipelineSpecification.CullMode = CullMode::None;
        PipelineSpecification.FrontFace = FrontFace::Clockwise;
        PipelineSpecification.Viewport = { 0.0f, 400, 600, -400 };
        PipelineSpecification.Scissor.Extent = { 600, 400 };
        PipelineSpecification.Scissor.Offset = { 0, 0 };
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
        delete m_Pipeline;
        Application::OnExit();
    }

    void HelloTriangle::OnUpdate(float DeltaTime)
    {
        Application::OnUpdate(DeltaTime);

    }

    void HelloTriangle::OnRender(Renderer* Renderer)
    {
        Application::OnRender(Renderer);
        Renderer->BindPipeline(m_Pipeline);
        Renderer->DrawIndexed(nullptr, m_VertexBuffer, m_IndexBuffer, m_Shader);
    }
}
