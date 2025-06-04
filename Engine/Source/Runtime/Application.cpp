#include "Application.h"
#include "Window.h"
#include "Image.h"
#include "Log.h"
#include "Scene.h"
#include "CommandLine/ArgumentParser.h"
#include "Rendering/Renderer.h"
#include "Audio/AudioSystem.h"
#include "ResourceManager/ResourceManager.h"
#include "ResourceManager/ShaderManager.h"
#include "ResourceManager/TextureManager.h"
#include "ResourceManager/SoundManager.h"


#include <GLFW/glfw3.h>
#include <slang/slang.h>
#include <slang/slang-com-ptr.h>

#include "AssetDatabase.h"
#include "CameraSettings.h"
#include "DesktopWindow.h"
#include "ExitCode.h"
#include "ScopedTimer.h"
#include "TweenManager.h"
#include "Editor/DetailsPanel.h"
#include "Editor/PhysicsSettingsPanel.h"
#include "Editor/SceneHierarchyPanel.h"
#include "Editor/Selection.h"
#include "Editor/ViewportPanel.h"
#include "Editor/EditorGUI.h"
#include "Editor/ImGuiRenderer.h"


namespace Nova
{
    extern bool g_ApplicationRunning;
    extern ExitCode g_ExitCode;

    Application::Application(Array<const char*> Arguments) : m_Arguments(std::move(Arguments))
    {
        Memory::Memzero(m_Configuration);
        m_EngineDirectory = Path(NOVA_ENGINE_ROOT_DIR) / "Engine";
        m_EngineAssetsDirectory = m_EngineDirectory / "Assets";
    }

    bool Application::PreInitialize()
    {
#if defined(NOVA_PLATFORM_WINDOWS) || defined(NOVA_PLATFORM_LINUX) || defined(NOVA_PLATFORM_MACOS)
        if(!glfwInit())
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to initialize glfw!");
            return false;
        }
        NOVA_LOG(Application, Verbosity::Info, "Using GLFW version {}.{}.{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

        glfwSetErrorCallback([](const i32 Code, const char* Message)
        {
            NOVA_LOG(Application, Verbosity::Warning, "GLFW Error {}: {}", Code, Message);
        });
#endif

        NOVA_LOG(Application, Verbosity::Trace, "Creating window...");

        m_Configuration = CreateConfiguration();

        WindowCreateInfo WindowCreateInfo;
        WindowCreateInfo.Title = m_Configuration.AppName;
        WindowCreateInfo.Width = m_Configuration.WindowWidth;
        WindowCreateInfo.Height = m_Configuration.WindowHeight;
        WindowCreateInfo.Resizable = m_Configuration.WindowResizable;
        m_MainWindow = Window::Create(this);
        if (!m_MainWindow->Initialize(WindowCreateInfo))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create window!");
            return false;
        }

        if (DesktopWindow* Window = m_MainWindow->As<DesktopWindow>())
        {
            Window->Show();
        }

        NOVA_LOG(Application, Verbosity::Trace, "Creating Renderer...");
        m_Renderer = Renderer::Create(this, m_Configuration.Graphics.GraphicsApi);
        if(!m_Renderer->Initialize(RendererCreateInfo(m_MainWindow)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create renderer!");
            return false;
        }

        NOVA_LOG(Application, Verbosity::Trace, "Initializing Audio System...");
        m_AudioSystem = new AudioSystem(this);
        if(!m_AudioSystem->Init())
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create audio engine!");
            return false;
        }

        m_ShaderManager = new ShaderManager;
        m_TextureManager = new TextureManager;
        m_SoundManager = new SoundManager;
        m_AssetDatabase = new AssetDatabase;
        
        if(m_Configuration.WithEditor)
        {
            m_ImGuiRenderer = ImGuiRenderer::Create(m_Configuration.Graphics.GraphicsApi);
            if (!m_ImGuiRenderer->Initialize(this))
            {
                delete m_ImGuiRenderer;
                return false;
            }
        }

        if (createGlobalSession(&m_SlangSession); !m_SlangSession)
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create slang session!");
            return false;
        }
        return true;
    }

    void Application::Update()
    {
        m_UnscaledTime = glfwGetTime();
        m_Time = m_TimeScale * m_UnscaledTime;
        ScopedTimer FrameTimer([this](const f64 Duration)
        {
            m_UnscaledDeltaTime = Duration;
            m_DeltaTime = m_UnscaledDeltaTime * m_TimeScale;
        });

        m_MainWindow->Update((f32)m_UnscaledDeltaTime);
        OnUpdate((f32)m_UnscaledDeltaTime);
    }

    void Application::Render()
    {
        if (m_Renderer->BeginFrame() && g_ApplicationRunning)
        {
            OnFrameStarted(m_Renderer);
            m_Scene->OnFrameBegin(m_Renderer);

            m_Renderer->BeginRendering();
            OnRender(m_Renderer);
            m_Scene->OnRender(m_Renderer);

            if (m_Configuration.WithEditor)
            {
                m_ImGuiRenderer->BeginFrame();
                ImGui::DockSpaceOverViewport(ImGui::GetID("Dockspace"), ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
                OnGUI((f32)m_DeltaTime);
                m_ImGuiRenderer->EndFrame();
                m_ImGuiRenderer->Render();
            }
            m_Renderer->EndRendering();
            m_Renderer->EndFrame();
            m_Renderer->Present();
        }
    }

    void Application::OnInit()
    {
        JPH::RegisterDefaultAllocator();
        m_Scene = new Scene(this);
        m_Scene->SetName("Default Scene");
        m_Scene->OnInit();

        m_SceneHierarchyPanel = new SceneHierarchyPanel(m_Scene);
        m_DetailsPanel = new DetailsPanel(m_Scene);
        m_ViewportPanel = new ViewportPanel();
        m_PhysicsSettingsPanel = new PhysicsSettingsPanel();
        
        m_DetailsPanel->OnInit();
        m_SceneHierarchyPanel->OnInit();
        m_ViewportPanel->OnInit();
        m_PhysicsSettingsPanel->OnInit();

        auto& File = m_MenuBar.AddChild({ "File" });
        File.AddChild({ "Exit", nullptr, [this]{ RequireExit(ExitCode::Success); } });

        auto& View = m_MenuBar.AddChild({ "View" });
        View.AddChild({ "Details Panel", m_DetailsPanel->OpenedPtr() });
        View.AddChild({ "Scene Hierarchy", m_SceneHierarchyPanel->OpenedPtr() });
        View.AddChild({ "Viewport Window", m_ViewportPanel->OpenedPtr() });
        View.AddChild({ "Physics Settings", m_PhysicsSettingsPanel->OpenedPtr() });
        View.AddChild({ "ImGui Demo Window" , &m_ShowImGuiDemoWindow});
    }

    
    void Application::Run()
    {
        if(!PreInitialize())
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to init Nova Framework Core!");
            RequireExit(ExitCode::Error);
            return;
        }

        OnInit();

        while(m_IsRunning)
        {
            Update();
            Render();
        }
        
        OnExit();
    }

    void Application::OnStart()
    {
        m_Scene->ForEach([](const EntityHandle& EntityHandle)
        {
            EntityHandle->OnStart();
        });
    }
    
    void Application::OnRender(Renderer* Renderer)
    {

    }
    
    void Application::OnUpdate(f32 Delta)
    {     
        m_AudioSystem->OnUpdate();
        TweenManager::Get().Update(Delta);
        m_Scene->OnUpdate(Delta);
        m_DetailsPanel->OnUpdate(Delta);
        m_SceneHierarchyPanel->OnUpdate(Delta);
        m_ViewportPanel->OnUpdate(Delta);
    }

    
    void Application::OnGUI(f32 Delta)
    {
        const ImGuiIO& IO = ImGui::GetIO();

        UI::MainMenuMenuBar(m_MenuBar);
        m_DetailsPanel->OnInspectorGUI(IO);
        m_ViewportPanel->OnInspectorGUI(IO);
        m_SceneHierarchyPanel->OnInspectorGUI(IO);
        m_PhysicsSettingsPanel->OnInspectorGUI(IO);

        if (ImGui::Begin("Stats"))
        {
            ImGui::Text("Frame Time: %.2f ms", Delta * 1000.0f);
        }
        ImGui::End();
        
        if(m_ShowImGuiDemoWindow) ImGui::ShowDemoWindow(&m_ShowImGuiDemoWindow);
    }

    void Application::OnExit()
    {
        m_Scene->OnDestroy();
        delete m_Scene;
        
        if(m_Configuration.WithEditor)
        {
            m_ImGuiRenderer->Shutdown();
            delete m_ImGuiRenderer;
        }
            
        
        m_PhysicsSettingsPanel->OnDestroy();
        delete m_PhysicsSettingsPanel;
        m_PhysicsSettingsPanel = nullptr;
        
        
        m_ViewportPanel->OnDestroy();
        delete m_ViewportPanel;
        m_ViewportPanel = nullptr;
        
        m_DetailsPanel->OnDestroy();
        delete m_DetailsPanel;
        m_DetailsPanel = nullptr;
        
        m_SceneHierarchyPanel->OnDestroy();
        delete m_SceneHierarchyPanel;
        m_SceneHierarchyPanel = nullptr;

        m_AssetDatabase->UnloadAll();
        delete m_AssetDatabase;
        m_AssetDatabase = nullptr;

        m_TextureManager->UnloadAll();
        delete m_TextureManager;
        m_TextureManager = nullptr;
        
        m_ShaderManager->UnloadAll();
        delete m_ShaderManager;
        m_ShaderManager = nullptr;
        
        m_SoundManager->UnloadAll();
        delete m_SoundManager;
        m_SoundManager = nullptr;
        
        m_AudioSystem->Destroy();
        delete m_AudioSystem;
        m_AudioSystem = nullptr;
        
        m_Renderer->Destroy();
        delete m_Renderer;
        m_Renderer = nullptr;

        slang::shutdown();
        
        m_MainWindow->Destroy();
        delete m_MainWindow;
        m_MainWindow = nullptr;
        
        glfwTerminate();
    }

    void Application::OnFrameStarted(Renderer* Renderer)
    {

    }

    const ApplicationConfiguration& Application::GetConfiguration() const
    {
        return m_Configuration;
    }

    const GraphicsSettings& Application::GetGraphicsSettings() const
    {
        return m_Configuration.Graphics;
    }

    const AudioSettings& Application::GetAudioSettings() const
    {
        return m_Configuration.Audio;
    }


    Window* Application::GetWindow() const
    {
        return m_MainWindow;
    }

    void Application::RequireExit(const ExitCode ExitCode)
    {
        NOVA_LOG(Application, Verbosity::Warning, "Exit required. Cleaning...");
        m_IsRunning = false;
        g_ApplicationRunning = false;
        g_ExitCode = ExitCode;
    }

    f64 Application::GetTimeScale() const
    {
        return m_TimeScale;
    }

    void Application::SetTimeScale(const f64 TimeScale)
    {
        m_TimeScale = Math::Clamp(TimeScale, 0.0, 1.0);
    }

    f64 Application::GetTime() const
    {
        return m_Time;
    }
    
    f64 Application::GetDeltaTime() const
    {
        return m_DeltaTime;
    }
    
    f64 Application::GetUnscaledDeltaTime() const
    {
        return m_UnscaledDeltaTime;
    }
    

    f64 Application::GetUnscaledTime() const
    {
        return m_UnscaledTime;
    }

    Path const& Application::GetEngineDirectory() const
    {
        return m_EngineDirectory;
    }

    Path const& Application::GetEngineAssetsDirectory() const
    {
        return m_EngineAssetsDirectory;
    }

    Renderer* Application::GetRenderer() const
    {
        return m_Renderer;
    }

    Scene* Application::GetScene() const
    {
        return m_Scene;
    }

    AudioSystem* Application::GetAudioSystem() const
    {
        return m_AudioSystem;
    }

    ShaderManager* Application::GetShaderManager() const
    {
        return m_ShaderManager;
    }

    TextureManager* Application::GetTextureManager() const
    {
        return m_TextureManager;
    }

    AssetDatabase* Application::GetAssetDatabase() const
    {
        return m_AssetDatabase;
    }

    SlangSession* Application::GetSlangSession() const
    {
        return m_SlangSession;
    }

    SceneHierarchyPanel* Application::GetSceneHierarchyPanel() const
    {
        return m_SceneHierarchyPanel;
    }

    DetailsPanel* Application::GetDetailsPanel() const
    {
        return m_DetailsPanel;
    }

    ViewportPanel* Application::GetViewportPanel() const
    {
        return m_ViewportPanel;
    }

    
}
