#include "Application.h"
#include "Window.h"
#include "Image.h"
#include "Log.h"
#include "Color.h"
#include "LogVerbosity.h"
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

#include "CameraSettings.h"
#include "Cursors.h"
#include "ExitCode.h"
#include "ScopedTimer.h"
#include "Editor/DetailsPanel.h"
#include "Editor/PhysicsSettingsPanel.h"
#include "Editor/SceneHierarchyPanel.h"
#include "Editor/Selection.h"
#include "Editor/ViewportPanel.h"

#include "Components/Camera.h"
#include "Components/Rendering/ModelRenderer.h"
#include "Editor/EditorGUI.h"
#include "Rendering/FrameBuffer.h"


#define NOVA_LOG_WINDOW_CALLBACKS

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
        // Init GLFW
        if(!glfwInit())
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to initialize glfw!");
            return false;
        }
        
        NOVA_LOG(Application, Verbosity::Info, "Using GLFW version {}.{}.{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);


        glfwSetErrorCallback([](int code, const char* message)
        {
            NOVA_LOG(Application, Verbosity::Warning, "[GLFW] Error {}: {}", code, message);
        });
        
        // Create Window, set its callbacks
        NOVA_LOG(Application, Verbosity::Trace, "Creating window...");

        m_Configuration = CreateConfiguration();
        m_MainWindow = new Window(m_Configuration);


        glfwSetWindowUserPointer(m_MainWindow->GetNativeWindow(), this);
        // Set window callbacks
        glfwSetWindowCloseCallback(m_MainWindow->GetNativeWindow(), [](GLFWwindow*)
        {
            g_Application->RequireExit(ExitCode::Error);
        });

        glfwSetWindowFocusCallback(m_MainWindow->GetNativeWindow(), [](GLFWwindow* window, int focus){
            Application* application = (Application*)glfwGetWindowUserPointer(window);
            application->m_MainWindow->m_HasFocus = (bool)focus;
#if defined(NOVA_LOG_WINDOW_CALLBACKS)
            const String Message = focus ? "Window focused" : "Window unfocused";
            NOVA_LOG(Application, Verbosity::Trace, "[WINDOW] {}", Message);
#endif
        });

        glfwSetWindowMaximizeCallback(m_MainWindow->GetNativeWindow(), [](GLFWwindow* window, int maximized){
            Application* application = (Application*)glfwGetWindowUserPointer(window);
            application->m_MainWindow->m_Maximized = (bool)maximized;
            application->GetRenderer()->ShouldRecreateSwapchain = true;
#if defined(NOVA_LOG_WINDOW_CALLBACKS)
            if(maximized) NOVA_LOG(Application, Verbosity::Trace, "[WINDOW] maximized");
#endif
        });

        glfwSetWindowPosCallback(m_MainWindow->GetNativeWindow(), [](GLFWwindow* window, int x, int y){
            Application* application = (Application*)glfwGetWindowUserPointer(window);
            application->m_MainWindow->m_PositionX = x;
            application->m_MainWindow->m_PositionY = y;
        });

        glfwSetWindowSizeCallback(m_MainWindow->GetNativeWindow(), [](GLFWwindow* window, int width, int height){
            const Application* application = (Application*)glfwGetWindowUserPointer(window);
            application->m_MainWindow->m_Width = width;
            application->m_MainWindow->m_Height = height;
            Renderer* Renderer = application->GetRenderer();
            //Renderer->SetViewportRect({0, 0}, {(f32)width, (f32)height});
            Renderer->ShouldRecreateSwapchain = true;
        });

        glfwSetWindowIconifyCallback(m_MainWindow->GetNativeWindow(), [](GLFWwindow* window, int iconified)
        {
            Application* application = (Application*)glfwGetWindowUserPointer(window);
            application->m_MainWindow->m_Minimized = (bool)iconified;
#if defined(NOVA_LOG_WINDOW_CALLBACKS)
            if(iconified) NOVA_LOG(Application, Verbosity::Trace, "[WINDOW] minimized");
#endif
        });

        glfwSetKeyCallback(m_MainWindow->GetNativeWindow(), [](GLFWwindow* window, int key, int, int action, int)
        {
            switch (action)
            {
            case GLFW_PRESS:
                if(ApplicationEvents::OnKeyEvent.IsBound()) ApplicationEvents::OnKeyEvent.Broadcast((KeyCode)key, InputState::Pressed);
                Input::s_KeyStates[(KeyCode)key] = InputState::Pressed;
                break;
            case GLFW_RELEASE:
                if(ApplicationEvents::OnKeyEvent.IsBound()) ApplicationEvents::OnKeyEvent.Broadcast((KeyCode)key, InputState::Released);
                Input::s_KeyStates[(KeyCode)key] = InputState::Released;
                break;
            default:
                break;
            }
        });

        glfwSetMouseButtonCallback(m_MainWindow->GetNativeWindow(), [](GLFWwindow *window, int button, int action, int)
        {
            switch (action)
            {
            case GLFW_PRESS:
                if(ApplicationEvents::OnMouseButtonEvent.IsBound()) ApplicationEvents::OnMouseButtonEvent.Broadcast((MouseButton)button, InputState::Pressed);
                Input::s_MouseButtonStates[(MouseButton)button] = InputState::Pressed;
                break;
            case GLFW_RELEASE:
                if(ApplicationEvents::OnMouseButtonEvent.IsBound()) ApplicationEvents::OnMouseButtonEvent.Broadcast((MouseButton)button, InputState::Released);
                Input::s_MouseButtonStates[(MouseButton)button] = InputState::Released;
                break;
            }
        });

        glfwSetJoystickCallback([](const int JoystickID, const int Event)
        {
            if(!glfwJoystickIsGamepad(JoystickID)) return;
            
            if(Event == GLFW_CONNECTED)
            {
                const StringView GamepadName = glfwGetJoystickName(JoystickID);
                NOVA_LOG(Application, Verbosity::Warning, "Gamepad {} connected: {}", JoystickID, GamepadName);
            }
            else if(Event == GLFW_DISCONNECTED)
            {
                const StringView GamepadName = glfwGetJoystickName(JoystickID);
                NOVA_LOG(Application, Verbosity::Warning, "Gamepad {} connected: {}", JoystickID, GamepadName);
            }
        });
        
        
        
        NOVA_LOG(Application, Verbosity::Info, "Window successfully created!");
        
        
        if(!m_MainWindow->IsValid())
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create window!");
            return false;
        }

        NOVA_LOG(Application, Verbosity::Trace, "Creating Renderer...");
        m_Renderer = Renderer::Create(this, m_Configuration.Graphics.GraphicsApi);
        if(!m_Renderer->Initialize())
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create renderer!");
            return false;
        }
        
        NOVA_LOG(Application, Verbosity::Info, "Renderer created!");

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

    void Application::OnInit()
    {
        //m_ShaderManager->Load("Sprite",       m_EngineAssetsDirectory / "Shaders/Sprite.glsl");
        //m_ShaderManager->Load("UniformColor", m_EngineAssetsDirectory / "Shaders/UniformColor.glsl");
        //m_ShaderManager->Load("Circle",       m_EngineAssetsDirectory / "Shaders/Circle.glsl");

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

        DialogFilters NovaSceneFilters;
        NovaSceneFilters.AddFilter("Nova Scene", {"hbsn", "htscn"});
        NovaSceneFilters.AddFilter(DialogFilters::All);
        
        auto& File = m_MenuBar.AddChild({ "File" });
        
        File.AddChild({ "Open Scene" , nullptr, [this, NovaSceneFilters]
        {
            const Path Filepath = File::OpenFileDialog("Open Scene", "", NovaSceneFilters);
            ApplicationEvents::OnSceneLoadEvent.Broadcast(Filepath);
        }});
        File.AddChild({ "Save Scene" });
        File.AddChild({ "Save Scene As", nullptr, [this, NovaSceneFilters]
        {
            const Path Filepath = File::SaveFileDialog("Save scene as...", "", NovaSceneFilters);
            ApplicationEvents::OnSceneSaveEvent.Broadcast(Filepath);
        }});
        File.AddChild({ "Exit", nullptr, [this]{ RequireExit(ExitCode::Success); } });

        auto& Edit = m_MenuBar.AddChild({ "Edit" });
        Edit.AddChild({ "Undo" });
        Edit.AddChild({ "Redo" });
        Edit.AddChild({ "Preferences" });

        
        auto& View = m_MenuBar.AddChild({ "View" });
        View.AddChild({ "Details Panel", m_DetailsPanel->OpenedPtr() });
        View.AddChild({ "Scene Hierarchy", m_SceneHierarchyPanel->OpenedPtr() });
        View.AddChild({ "Viewport Window", m_ViewportPanel->OpenedPtr() });
        View.AddChild({ "Physics Settings", m_PhysicsSettingsPanel->OpenedPtr() });
        View.AddChild({ "ImGui Demo Window" , &m_ShowImGuiDemoWindow});

        auto& Scene = m_MenuBar.AddChild({ "Scene"});
        Scene.AddChild({ "Rename" });
        Scene.AddChild({ "Create Entity", nullptr, [this]
        {
            const EntityHandle Entity = m_Scene->CreateEntity("New Entity");
            Selection::SetEntity(Entity);
        }});
        Scene.AddChild({ "Create Camera", nullptr, [this]
        {
            const f32 Width = GetWindow()->GetWidth();
            const f32 Height = GetWindow()->GetHeight();
            const CameraSettings Settings = CameraSettings::DefaultOrthographic.WithDimension(Width, Height);
            EntityHandle Entity = m_Scene->CreateEntity("Camera");
            Camera* Camera = Entity->AddComponent<class Camera>();
            Camera->Settings = Settings;
            m_Renderer->SetCurrentCamera(Camera);
            Selection::SetEntity(Entity);
        }});
        
        auto& Misc = m_MenuBar.AddChild({ "Misc" });
        auto& Shaders = Misc.AddChild({ "Shaders" });
        Shaders.AddChild({ "Reload All", nullptr, [this]
        {
            m_ShaderManager->ReloadAll();
        }});
        auto& Tools = m_MenuBar.AddChild({ "Tools" });
        Tools.AddChild({ "Open Model", nullptr, [this]
        {
            ModelRenderer* RendererComponent = m_Scene->GetAllComponents<ModelRenderer>()[0];
            RendererComponent->OpenFile();
        } });
        
        
        ApplicationEvents::OnInitEvent.Broadcast();
    }

    
    void Application::Run()
    {
        if(!PreInitialize())
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to init Nova Framework Core!");
            RequireExit(ExitCode::Error);
            return;
        }

        m_MainWindow->Show();
        OnInit();

        static Vector2 CurrentMousePosition;
        static Vector2 LastMousePosition;
        while(m_IsRunning)
        {
            m_UnscaledTime = glfwGetTime();
            m_Time = m_TimeScale * m_UnscaledTime;
            ScopedTimer FrameTimer([this](const f64 Duration)
            {
                m_UnscaledDeltaTime = Duration;
                m_DeltaTime = m_UnscaledDeltaTime * m_TimeScale;
            });
            
            ApplicationEvents::OnFrameBegin.Broadcast();
            Input::ResetInputStates();
            Input::UpdateGamepads();
            CurrentMousePosition = Input::GetMousePosition();
            Input::s_DeltaMousePosition = LastMousePosition - CurrentMousePosition;
            LastMousePosition = CurrentMousePosition;
            glfwPollEvents();


            OnUpdate((float)m_UnscaledDeltaTime);
            OnRender(m_Renderer);
            ApplicationEvents::OnFrameEnd.Broadcast();
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
        switch (Renderer->GetGraphicsApi())
        {
        case GraphicsApi::None: return;
        case GraphicsApi::OpenGL:
            if(m_Configuration.WithEditor)
            {
                if(!m_ViewportPanel->IsAvailable()) return;

                // Clear all screen
                Renderer->ClearColor({0.08f, 0.08f, 0.08f, 1.0f});
                Renderer->ClearDepth(0.0f);

                m_ViewportPanel->GetFrameBuffer()->Bind();
                if(m_Renderer->GetCurrentCamera())
                    m_Renderer->GetCurrentCamera()->Settings.SetDimensions(m_ViewportPanel->GetSize()); // Temp
                Renderer->SetViewportRect(Vector2::Zero, m_ViewportPanel->GetSize());
                const Camera* Camera = Renderer->GetCurrentCamera();
                Renderer->ClearColor(Camera->ClearColor);
                Renderer->ClearDepth(0.0f);
                m_Scene->OnRender(Renderer);
                m_ViewportPanel->GetFrameBuffer()->Unbind();

                if (m_Configuration.WithEditor)
                {
                    m_ImGuiRenderer->BeginFrame();
                    ImGui::DockSpaceOverViewport(ImGui::GetID("Dockspace"), ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
                    OnGUI((f32)m_DeltaTime);
                    m_ImGuiRenderer->EndFrame();
                    m_ImGuiRenderer->Render();
                }
                Renderer->Present();
            }
            else
            {
                const Camera* Camera = Renderer->GetCurrentCamera();
                Renderer->ClearColor(Camera->ClearColor);
                Renderer->ClearDepth(1.0f);
                m_Scene->OnRender(Renderer);
            }
        case GraphicsApi::Vulkan:
        case GraphicsApi::D3D12:
            if (Renderer->BeginFrame() && g_ApplicationRunning)
            {
                const Camera* Camera = Renderer->GetCurrentCamera();
                Renderer->Clear(Camera->ClearColor, 0.0f);
                m_Scene->OnRender(Renderer);

                if (m_Configuration.WithEditor)
                {
                    m_ImGuiRenderer->BeginFrame();
                    ImGui::DockSpaceOverViewport(ImGui::GetID("Dockspace"), ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
                    OnGUI((f32)m_DeltaTime);
                    m_ImGuiRenderer->EndFrame();
                    m_ImGuiRenderer->Render();
                }

                Renderer->EndFrame();
                Renderer->Present();
            }
        }
    }
    
    void Application::OnUpdate(f32 Delta)
    {     
        m_AudioSystem->OnUpdate();
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
            ImGui::Text("Frame Time: %.2f ms", Delta);
        }
        ImGui::End();
        
        if(m_ShowImGuiDemoWindow) ImGui::ShowDemoWindow(&m_ShowImGuiDemoWindow);
    }

    void Application::OnExit()
    {
        spDestroySession(m_SlangSession);
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
        
        m_MainWindow->Destroy();
        delete m_MainWindow;
        m_MainWindow = nullptr;
        
        glfwTerminate();
        ApplicationEvents::OnPostExitEvent.Broadcast();
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

    void Application::RequireExitAndRestart()
    {
        NOVA_LOG(Application, Verbosity::Warning, "Exit required. Cleaning... Application will restart");
        m_IsRunning = false;
        g_ApplicationRunning = true;
        g_ExitCode = ExitCode::Restarted;
    }

    void Application::SetCursorVisible(bool Visible) const
    {
        Cursor::SetCursorVisible(m_MainWindow, Visible);
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

    Path const& Application::GetApplicationDirectory() const
    {
        return m_ApplicationDirectory;
    }

    Path const& Application::GetAssetsDirectory() const
    {
        return m_ApplicationAssetsDirectory;
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
