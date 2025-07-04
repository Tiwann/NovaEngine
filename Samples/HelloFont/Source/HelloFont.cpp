#include "HelloFont.h"

#include "Components/Camera.h"
#include "Components/FreeFlyCameraComponent.h"
#include "Components/Transform.h"
#include "Components/Rendering/TextRenderer.h"
#include "Editor/Font.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/AssetDatabase.h"
#include "Runtime/EntityHandle.h"
#include "Runtime/EntryPoint.h"
#include "Runtime/Font.h"
#include "Runtime/Scene.h"

namespace Nova
{
    class FreeFlyCameraComponent;
}

using namespace Nova;

HelloFont::HelloFont(const Array<const char*>& Arguments) : Application(Arguments)
{

}

ApplicationConfiguration HelloFont::CreateConfiguration() const
{
    ApplicationConfiguration Configuration;
    Configuration.AppName = "Hello Font | Nova Engine";
    Configuration.WindowWidth = 1280;
    Configuration.WindowHeight = 720;
    Configuration.WindowResizable = true;
    Configuration.Audio.SampleRate = 44100;
    Configuration.Audio.BufferSize = 1024;
    Configuration.Audio.BufferCount = 4;
    Configuration.Graphics.GraphicsApi = GraphicsApi::Vulkan;
    Configuration.Graphics.Buffering = SwapchainBuffering::TripleBuffering;
    Configuration.Graphics.VSync = true;
    Configuration.WithEditor = true;
    return Configuration;
}


static EntityHandle CameraEntity = nullptr;

void HelloFont::OnInit()
{
    Application::OnInit();

    ShaderManager* ShaderManager = GetShaderManager();
    const Path FontShader = PathCombine(GetEngineShadersDirectory(), "Font.slang");
    ShaderManager->Load("Font", FontShader);

    CameraEntity = GetScene()->CreateEntity("Camera");
    Camera* CameraComponent = CameraEntity->AddComponent<Camera>();
    CameraComponent->SetSettings(CameraSettings::DefaultPerspective.WithFOV(45.0f));
    CameraComponent->GetTransform()->SetPosition({ 0.0f, 0.0f, 1.0f });


    FreeFlyCameraComponent* CameraController = CameraEntity->AddComponent<FreeFlyCameraComponent>();
    CameraController->SetCamera(CameraComponent);

    Renderer* Renderer = GetRenderer();
    Renderer->SetCurrentCamera(CameraComponent);

    GetScene()->CreateEntity("Text")->AddComponent<TextRenderer>();
}

NOVA_DEFINE_APPLICATION_CLASS(HelloFont);
