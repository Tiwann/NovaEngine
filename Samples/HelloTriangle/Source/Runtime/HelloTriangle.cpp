#include "HelloTriangle.h"
#include "Runtime/EntryPoint.h"
#include "CommandLine/ArgumentParser.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Shader.h"
#include "Rendering/Vertex.h"
#include "Rendering/VertexBuffer.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/TweenManager.h"

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
        Configuration.AppName = "Nova Editor";
        Configuration.WindowWidth = 1600;
        Configuration.WindowHeight = 900;
        Configuration.WindowResizable = true;
        Configuration.Audio.SampleRate = 44100;
        Configuration.Audio.BufferSize = 1024;
        Configuration.Audio.BufferCount = 4;
        Configuration.Graphics.GraphicsApi = GraphicsApi::Vulkan;
        Configuration.Graphics.BufferType = SwapchainBuffering::DoubleBuffering;
        Configuration.Graphics.VSync = false;
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
            Vertex(Vector3(-0.5f, +0.5f, 0.0f), Vector2::Zero, Vector3::Zero, Color::Red),
            Vertex(Vector3(+0.0f, -0.5f, 0.0f), Vector2::Zero, Vector3::Zero, Color::Green),
            Vertex(Vector3(+0.5f, +0.5f, 0.0f), Vector2::Zero, Vector3::Zero, Color::Blue),
        };

        const Array<u32> Indices { 0, 1, 2 };

        m_VertexBuffer = VertexBuffer::Create(Vertices, GraphicsApi::Vulkan);
        m_IndexBuffer = IndexBuffer::Create(Indices, GraphicsApi::Vulkan);
    }

    void HelloTriangle::OnExit()
    {
        delete m_VertexBuffer;
        delete m_IndexBuffer;
        Application::OnExit();
    }

    void HelloTriangle::OnRender(Renderer* Renderer)
    {
        Application::OnRender(Renderer);
        Renderer->DrawIndexed(DrawMode::Triangles, nullptr, m_VertexBuffer, m_IndexBuffer, m_Shader);
    }
}
