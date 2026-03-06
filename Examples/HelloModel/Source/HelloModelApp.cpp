#include "HelloModelApp.h"
#include "FreeFlyCameraComponent.h"
#include "Components/Rendering/StaticMeshRenderer.h"
#include "Rendering/CommandBuffer.h"
#include "Runtime/Log.h"
#include "Runtime/LogCategory.h"
#include "Runtime/Path.h"
#include "Runtime/Scene.h"
#include "Runtime/StaticMesh.h"
#include "Components/Camera.h"
#include "Components/Rendering/AmbientLight.h"
#include "Components/Rendering/DirectionalLight.h"

using namespace Nova;

NOVA_DECLARE_LOG_CATEGORY_STATIC(HelloModel, "HelloModel")

extern "C" Application* CreateApplication(int argc, char** argv)
{
    return new HelloModelApp(argc, argv);
}

Nova::ApplicationConfiguration HelloModelApp::GetConfiguration() const
{
    ApplicationConfiguration configuration;
    configuration.applicationName = "HelloTriangle";
    configuration.vsync = true;
    configuration.windowFlags = WindowCreateFlagBits::Default | WindowCreateFlagBits::Resizable;
    configuration.windowWidth = 800;
    configuration.windowHeight = 600;
    return configuration;
}

void HelloModelApp::OnInit()
{
    AssetDatabase& assetDatase = GetAssetDatabase();
    const String modelFilepath = Path::OpenFileDialog("Open a 3D Model", Path::GetDesktopDirectory(), DialogFilters::ModelFilters, *GetWindow());
    if (!Path::Exists(modelFilepath))
    {
        NOVA_LOG(HelloModel, Verbosity::Error, "Mesh loading aborted.");
        Exit();
    }

    Ref<StaticMesh> staticMesh = assetDatase.CreateAsset<StaticMesh>("StaticMesh");
    if (!staticMesh->LoadFromFile(modelFilepath, true))
    {
        NOVA_LOG(HelloModel, Verbosity::Error, "Failed to load mesh from file: {}", modelFilepath);
        Exit();
        return;
    }

    SceneManager* sceneManager = GetSceneManager();
    Scene* scene = sceneManager->CreateSceneAndSetActive(this, "MainScene");

    m_ModelEntity = scene->CreateEntity("Model");
    StaticMeshRenderer* renderer = m_ModelEntity->AddComponent<StaticMeshRenderer>();


    EntityHandle cameraEntity = scene->CreateEntity("Camera");
    Camera* camera = cameraEntity->AddComponent<Camera>();
    FreeFlyCameraComponent* cameraMovement = cameraEntity->AddComponent<FreeFlyCameraComponent>();
    cameraMovement->SetCamera(camera);

    EntityHandle lightEntity = scene->CreateEntity("Light");
    lightEntity->AddComponent<DirectionalLight>();
    lightEntity->AddComponent<AmbientLight>();
    renderer->SetStaticMesh(staticMesh);
    scene->OnInit();
}

Nova::RenderDeviceType HelloModelApp::GetRenderDeviceType() const
{
    return RenderDeviceType::Vulkan;
}