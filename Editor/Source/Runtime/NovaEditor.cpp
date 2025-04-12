#include "NovaEditor.h"
#include "Runtime/EntryPoint.h"
#include "CommandLine/ArgumentParser.h"
#include "Components/Camera.h"
#include "Runtime/TweenManager.h"

NOVA_DEFINE_APPLICATION_CLASS(NovaEditor)

namespace Nova
{
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
        Configuration.Graphics.GraphicsApi = GraphicsApi::OpenGL;
        Configuration.Graphics.BufferType = SwapchainBuffering::DoubleBuffering;
        Configuration.Graphics.VSync = false;
        Configuration.WithEditor = true;
        return Configuration;
    }

    Camera* Camera = nullptr;
    void NovaEditor::OnInit()
    {
        Application::OnInit();

        const EntityHandle CameraEntity = GetScene()->CreateEntity("Camera");
        Camera = CameraEntity->AddComponent<class Camera>();
        GetRenderer()->SetCurrentCamera(Camera);
        Camera->ClearColor = Color::Red;
    }

    void NovaEditor::OnExit()
    {
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
