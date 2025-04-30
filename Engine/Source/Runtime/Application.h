#pragma once
#include "ApplicationConfiguration.h"
#include "ExitCode.h"
#include "Containers/TreeNode.h"
#include "Containers/String.h"
#include "LogCategory.h"
#include "Containers/MulticastDelegate.h"
#include "Input/Input.h"
#include "Editor/Menu.h"


NOVA_DECLARE_LOG_CATEGORY_STATIC(Application, "APPLICATION");

namespace slang
{
    struct IGlobalSession;
}
typedef slang::IGlobalSession SlangSession;

namespace Nova
{
    class TextureManager;
    class ShaderManager;
    class Renderer;
    class Window;
    class Image;
    class Scene;
    class Entity;
    class AudioSystem;
    class ViewportPanel;
    class DetailsPanel;
    class SceneHierarchyPanel;
    class PhysicsSettingsPanel;
    class AssetDatabase;
    class ModuleManager;
    class SoundManager;
    class ImGuiRenderer;
    
    struct ApplicationEvents
    {
        using ApplicationDelegate = MulticastDelegate<void()>;
        static inline ApplicationDelegate OnInitEvent;
        static inline ApplicationDelegate OnStartEvent;
        static inline ApplicationDelegate OnStopEvent;
        static inline ApplicationDelegate OnPreExitEvent;
        static inline ApplicationDelegate OnPostExitEvent;
        static inline ApplicationDelegate OnFrameBegin;
        static inline ApplicationDelegate OnFrameEnd;
        
        static inline MulticastDelegate<void(Path const&)> OnSceneSaveEvent;
        static inline MulticastDelegate<void(Path const&)> OnSceneLoadEvent;

        using KeyDelegate = MulticastDelegate<void(KeyCode, InputState)>;
        static inline KeyDelegate OnKeyEvent;

        static inline MulticastDelegate<void(KeyCode)> OnKeyPressedEvent;
        static inline MulticastDelegate<void(KeyCode)> OnKeyReleasedEvent;

        using MouseButtonDelegate = MulticastDelegate<void(MouseButton, InputState)>;
        static inline MouseButtonDelegate OnMouseButtonEvent;

        using GamepadButtonDelegate = MulticastDelegate<void(size_t ID, GamepadButton Button, InputState State)>;
        static inline GamepadButtonDelegate OnGamepadButtonEvent;

        using GamepadAxisDelegate = MulticastDelegate<void(size_t ID, size_t Axis, f32 Value)>;
        static inline GamepadAxisDelegate OnGamepadAxisEvent;
    };
    
    class Application
    {
    public:
        Application(Array<const char*> Arguments);
        virtual ~Application() = default;

        /* Application is not copyable nor moveable */
        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;


        void Run();

        /* Application Interface */
        virtual void OnInit();
        virtual void OnStart();
        virtual void OnExit();
        virtual void OnFrameStarted(Renderer* Renderer);
        virtual void OnRender(Renderer* Renderer);
        virtual void OnUpdate(f32 Delta);
        virtual void OnGUI(f32 Delta);

        virtual ApplicationConfiguration CreateConfiguration() const = 0;
        
        const ApplicationConfiguration&  GetConfiguration() const;
        const GraphicsSettings&          GetGraphicsSettings() const;
        const AudioSettings&             GetAudioSettings() const;

        
        Window*                 GetWindow() const;
        Renderer*               GetRenderer() const;
        template<typename RendererBackendType> requires IsBaseOfValue<Renderer, RendererBackendType>
        RendererBackendType*    GetRenderer() const { return dynamic_cast<RendererBackendType*>(GetRenderer()); }
        Scene*                  GetScene() const;
        AudioSystem*            GetAudioSystem() const;
        ShaderManager*          GetShaderManager() const;
        TextureManager*         GetTextureManager() const;
        AssetDatabase*          GetAssetDatabase() const;
        SlangSession*           GetSlangSession() const;

        SceneHierarchyPanel*    GetSceneHierarchyPanel() const;
        DetailsPanel*           GetDetailsPanel() const;
        ViewportPanel*          GetViewportPanel() const;
        
        void                    RequireExit(ExitCode ExitCode);
        void                    RequireExitAndRestart();
        void                    SetCursorVisible(bool Visible) const;

        f64                     GetTimeScale() const;
        void                    SetTimeScale(f64 TimeScale);
        f64                     GetTime() const;
        f64                     GetDeltaTime() const;
        f64                     GetUnscaledDeltaTime() const;
        f64                     GetUnscaledTime() const;

        Path const&             GetEngineDirectory() const;
        Path const&             GetEngineAssetsDirectory() const;
    private:
        ////////////////////////////////////////////
        /// CORE COMPONENTS
        ////////////////////////////////////////////
        Window*                 m_MainWindow = nullptr;
        Array<Window*>          m_Windows;
        Renderer*               m_Renderer = nullptr;
        Scene*                  m_Scene = nullptr;
        AudioSystem*            m_AudioSystem = nullptr;
        ModuleManager*          m_ModuleManager = nullptr;
        SlangSession*           m_SlangSession = nullptr;
    protected:
        ////////////////////////////////////////////
        /// RESOURCES MANAGERS
        ////////////////////////////////////////////
        ShaderManager*          m_ShaderManager = nullptr;
        TextureManager*         m_TextureManager = nullptr;
        SoundManager*           m_SoundManager = nullptr;
        AssetDatabase*          m_AssetDatabase = nullptr;
    private:

        ////////////////////////////////////////////
        /// UI COMPONENTS
        ////////////////////////////////////////////
        ImGuiRenderer*          m_ImGuiRenderer = nullptr;
        TreeNode<MenuItem>      m_MenuBar;
        SceneHierarchyPanel*    m_SceneHierarchyPanel = nullptr;
        DetailsPanel*           m_DetailsPanel = nullptr;
        ViewportPanel*          m_ViewportPanel = nullptr;
        PhysicsSettingsPanel*   m_PhysicsSettingsPanel = nullptr;
        

    private:
        Array<const char*> m_Arguments;
        ApplicationConfiguration m_Configuration;
        bool m_IsRunning = true;
        bool m_Start = false;
        bool m_ShowImGuiDemoWindow = false;
        f64  m_TimeScale = 1.0;
        f64  m_Time = 0.0;
        f64  m_DeltaTime = 0.0;
        f64  m_UnscaledDeltaTime = 0.0;
        f64  m_UnscaledTime = 0.0;
        Path m_EngineDirectory;
        Path m_EngineAssetsDirectory;
        bool PreInitialize();
    };

    inline Application* g_Application;
}
