#include "HelloCube.h"
#include "CommandLine/ArgumentParser.h"
#include "Platform/Vulkan/VulkanTexture2D.h"
#include "Rendering/Shader.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/EntityHandle.h"
#include "Runtime/EntryPoint.h"
#include "Runtime/Scene.h"

#include "Components/Camera.h"
#include "Components/EditorCameraController.h"
#include "Components/Transform.h"
#include "Components/Rendering/StaticMeshRenderer.h"
#include "Components/Rendering/DirectionalLight.h"
#include "Components/Rendering/AmbientLight.h"


NOVA_DEFINE_APPLICATION_CLASS(HelloCube)


namespace Nova
{
    HelloCube::HelloCube(const Array<const char*>& Arguments) : Application(Arguments)
    {

    }

    ApplicationConfiguration HelloCube::CreateConfiguration() const
    {
        ApplicationConfiguration Configuration;
        Configuration.AppName = "Hello Cube | Nova Engine";
        Configuration.WindowWidth = 1280;
        Configuration.WindowHeight = 720;
        Configuration.WindowResizable = true;
        Configuration.Audio.SampleRate = 44100;
        Configuration.Audio.BufferSize = 1024;
        Configuration.Audio.BufferCount = 4;
        Configuration.Graphics.GraphicsApi = GraphicsApi::Vulkan;
        Configuration.Graphics.BufferType = SwapchainBuffering::TripleBuffering;
        Configuration.Graphics.VSync = true;
        Configuration.WithEditor = true;
        return Configuration;
    }

    static EntityHandle MeshEntity = nullptr;
    static EntityHandle LightEntity = nullptr;
    static EntityHandle CameraEntity = nullptr;

    void HelloCube::OnInit()
    {
        Application::OnInit();

        ShaderManager* ShaderManager = GetShaderManager();
        ShaderManager->Load("HelloCube", Path(NOVA_APPLICATION_ROOT_DIR) / "Assets/Shaders/HelloCube.slang");

        Scene* Scene = GetScene();
        MeshEntity = Scene->CreateEntity("Model");

        StaticMeshRenderer* MeshRenderer = MeshEntity->AddComponent<StaticMeshRenderer>();
        MeshEntity->GetTransform()->SetPosition(0.0f, -0.8f, -1.9f);
        MeshEntity->GetTransform()->SetRotation(0.0f, 21.0f, 0.0f);
        MeshRenderer->OpenFile();

        LightEntity = Scene->CreateEntity("Light");
        DirectionalLight* DirLight = LightEntity->AddComponent<DirectionalLight>();
        DirLight->SetIntensity(1.0f);
        DirLight->SetColor(Color::White.WithLightness(0.5f));
        DirLight->GetTransform()->SetRotation(Vector3(180.0f, -152.0f, 0.0f));

        AmbientLight* AmbLight = LightEntity->AddComponent<AmbientLight>();
        AmbLight->SetColor(Color::White.WithLightness(0.15f));

        CameraEntity = GetScene()->CreateEntity("Camera");
        Camera* CameraComponent = CameraEntity->AddComponent<Camera>();
        CameraComponent->SetSettings(CameraSettings::DefaultPerspective.WithFOV(45.0f));
        CameraComponent->GetTransform()->SetPosition({ 0.0f, 0.0f, 1.0f });

        EditorCameraController* CameraController = CameraEntity->AddComponent<EditorCameraController>();
        CameraController->SetCamera(CameraComponent);

        Renderer* Renderer = GetRenderer();
        Renderer->SetCurrentCamera(CameraComponent);
    }
}
