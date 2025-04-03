#include "EditorApplication.h"
#include "Runtime/EntryPoint.h"
#include "CommandLine/ArgumentParser.h"


NOVA_DEFINE_APPLICATION_CLASS(EditorApplication)

namespace Nova
{
    EditorApplication::EditorApplication(const Array<const char*>& Arguments) : Application(Arguments)
    {
        ArgumentParser ArgParser("NovaEditor", Arguments, ArgumentParserSettings::WindowsStyle);

        const CommandLineOption HelpOption { 'h', "help", false, "Show available commands" };
        const CommandLineOption ProjectOption { 'p', "project", false, "Open specified project" };
        ArgParser.AddOptions({ HelpOption, ProjectOption });
        ArgParser.Parse();

        String ProjectPath = ArgParser.GetString(ProjectOption);

    }

    ApplicationConfiguration EditorApplication::CreateConfiguration(ApplicationConfigurationSerializer& Serializer) const
    {
        ApplicationConfiguration Configuration;
        Configuration.AppName = "Nova Editor";
        Configuration.WindowWidth = 1600;
        Configuration.WindowHeight = 900;
        Configuration.Audio.SampleRate = 44100;
        Configuration.Audio.BufferSize = 1024;
        Configuration.Audio.BufferCount = 4;
        Configuration.Graphics.BufferType = GraphicsSettings::SwapchainBufferType::DoubleBuffering;
        Configuration.Graphics.VSync = false;
        return Configuration;
    }

    void EditorApplication::OnInit()
    {
        Application::OnInit();
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
    }

}
