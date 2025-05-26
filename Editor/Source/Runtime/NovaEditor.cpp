#include "NovaEditor.h"


#include "Runtime/EntryPoint.h"
#include "CommandLine/ArgumentParser.h"
#include "Components/Camera.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Shader.h"
#include "Rendering/Vertex.h"
#include "Rendering/VertexBuffer.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/TweenManager.h"
#include "Components/Rendering/StaticMeshRenderer.h"
#include <vulkan/vulkan.h>

NOVA_DEFINE_APPLICATION_CLASS(NovaEditor)

namespace Nova
{
    class StaticMeshRenderer;

    NovaEditor::NovaEditor(const Array<const char*>& Arguments) : Application(Arguments)
    {
        const CommandLineOption ProjectOption { 'p', "project", false, false, "Open specified project" };
        ArgumentParser ArgParser("NovaEditor", Arguments, ArgumentParserSettings::WindowsStyle, ProjectOption);
        if (ArgParser.GetBool(ProjectOption))
        {
            m_CurrentProject = MakeShared<Project>(*ArgParser.GetString(ProjectOption));
        }
    }

    ApplicationConfiguration NovaEditor::CreateConfiguration() const
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
        Configuration.Graphics.Buffering = SwapchainBuffering::DoubleBuffering;
        Configuration.Graphics.VSync = false;
        Configuration.WithEditor = false;
        return Configuration;
    }

    static Camera* s_Camera = nullptr;
    static Shader* s_HelloTriangleShader = nullptr;
    static VertexBuffer* s_VertexBuffer = nullptr;
    static IndexBuffer* s_IndexBuffer = nullptr;

    void NovaEditor::OnInit()
    {
        Application::OnInit();

        const EntityHandle CameraEntity = GetScene()->CreateEntity("Camera");
        s_Camera = CameraEntity->AddComponent<Camera>();
        GetRenderer()->SetCurrentCamera(s_Camera);
        s_Camera->ClearColor = Color::Black;

        const Path ShaderPath = GetEngineAssetsDirectory() / "Shaders/NovaEditor.slang";
        s_HelloTriangleShader = m_ShaderManager->Load("NovaEditor", ShaderPath);

        const Array<Vertex> Vertices
        {
            Vertex(Vector3(-0.5f, +0.5f, 0.0f ), Vector2::Zero, Vector3::Zero, Color::Red),
            Vertex(Vector3(+0.0f, -0.5f, 0.0f ), Vector2::Zero, Vector3::Zero, Color::Green),
            Vertex(Vector3(+0.5f, +0.5f, 0.0f ), Vector2::Zero, Vector3::Zero, Color::Blue),
        };

        const Array<u32> Indices
        {
            0, 1, 2,
        };

        s_VertexBuffer = VertexBuffer::Create(Vertices, GraphicsApi::Vulkan);
        s_IndexBuffer = IndexBuffer::Create(Indices, GraphicsApi::Vulkan);
    }

    void NovaEditor::OnExit()
    {
        delete s_VertexBuffer;
        delete s_IndexBuffer;
        Application::OnExit();
    }

    void NovaEditor::OnRender(Renderer* Renderer)
    {
        Application::OnRender(Renderer);

    }

    void NovaEditor::OnUpdate(f32 Delta)
    {
        Application::OnUpdate(Delta);
        for (EditorWindow* EditorWindow : m_EditorWindows)
        {
            EditorWindow->OnUpdate(Delta);
        }
    }

    void NovaEditor::OpenProject(const Path& ProjectPath)
    {
        m_CurrentProject.reset();
        m_CurrentProject = MakeShared<Project>(ProjectPath);
    }
}
