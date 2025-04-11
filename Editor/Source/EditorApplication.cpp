#include "EditorApplication.h"
#include "Runtime/EntryPoint.h"
#include "CommandLine/ArgumentParser.h"
#include "Components/Camera.h"
#include "Runtime/TweenManager.h"

NOVA_DEFINE_APPLICATION_CLASS(EditorApplication)

namespace Nova
{
    EditorApplication::EditorApplication(const Array<const char*>& Arguments) : Application(Arguments)
    {
        const CommandLineOption ProjectOption { 'p', "project", false, false, "Open specified project" };
        ArgumentParser ArgParser("NovaEditor", Arguments, ArgumentParserSettings::WindowsStyle, ProjectOption);
        if (ArgParser.GetBool(ProjectOption))
        {
            m_CurrentProject = MakeShared<Project>(*ArgParser.GetString(ProjectOption));
        }
    }

    ApplicationConfiguration EditorApplication::CreateConfiguration() const
    {
        ApplicationConfiguration Configuration;
        Configuration.AppName = "Nova Editor";
        Configuration.WindowWidth = 1600;
        Configuration.WindowHeight = 900;
        Configuration.WindowResizable = true;
        Configuration.Audio.SampleRate = 44100;
        Configuration.Audio.BufferSize = 1024;
        Configuration.Audio.BufferCount = 4;
        Configuration.Graphics.GraphicsApi = GraphicsApi::D3D12;
        Configuration.Graphics.BufferType = SwapchainBuffering::DoubleBuffering;
        Configuration.Graphics.VSync = false;
        Configuration.WithEditor = true;
        return Configuration;
    }

    Camera* Camera = nullptr;
    void EditorApplication::OnInit()
    {
        Application::OnInit();
        const EntityHandle CameraEntity = GetScene()->CreateEntity("Camera");
        Camera = CameraEntity->AddComponent<class Camera>();
        GetRenderer()->SetCurrentCamera(Camera);
        Camera->ClearColor = Color::Red;
    }

    void EditorApplication::OnExit()
    {
        Application::OnExit();
    }

    void EditorApplication::OnRender(Renderer* Renderer)
    {
        Application::OnRender(Renderer);
    }

    void EditorApplication::OnUpdate(f32 Delta)
    {
        Application::OnUpdate(Delta);
        TweenManager::Update(Delta);

        if (Input::GetKeyDown(KeyCode::Space))
        {
            static bool IsA = false;
            TweenManager::DoClearColor(Camera, IsA ? Color::Red : Color::Cyan, 1.0f);
            IsA = !IsA;
        }

    }

    void EditorApplication::OpenProject(const Path& ProjectPath)
    {
        m_CurrentProject.reset();
        m_CurrentProject = MakeShared<Project>(ProjectPath);
    }
}
