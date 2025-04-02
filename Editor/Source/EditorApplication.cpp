#include "EditorApplication.h"
#include "Runtime/EntryPoint.h"

NOVA_DEFINE_APPLICATION_CLASS(EditorApplication)

Nova::ApplicationConfiguration Nova::EditorApplication::CreateConfiguration(
    ApplicationConfigurationSerializer& Serializer) const
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

void Nova::EditorApplication::OnInit()
{
    Application::OnInit();
}

void Nova::EditorApplication::OnExit()
{
    Application::OnExit();
}

void Nova::EditorApplication::OnRender(Renderer* Renderer)
{
    Application::OnRender(Renderer);
}

void Nova::EditorApplication::OnUpdate(f32 Delta)
{
    Application::OnUpdate(Delta);
}
