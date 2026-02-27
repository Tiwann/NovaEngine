#pragma once
#include "Containers/String.h"
#include "Rendering/RenderDevice.h"
#include "Rendering/RenderPass.h"
#include "Rendering/RenderTarget.h"
#include "Rendering/ImGuiRenderer.h"
#include "SceneManager.h"
#include "Window.h"
#include "Ref.h"
#include "AssetDatabase.h"
#include "CmdLineArgs.h"

#include <cstdint>

namespace slang
{
    struct IGlobalSession;
}

namespace Nova
{
    class Window;
    class AudioSystem;
    class EditorWindow;
    class Event;

    struct ApplicationConfiguration
    {
        String applicationName = "Nova Application";
        uint32_t windowWidth = 600, windowHeight = 400;
        WindowCreateFlags windowFlags = WindowCreateFlagBits::Default;
        bool vsync = false;
        uint32_t msaaSamples = 8;
    };

    class Application
    {
    public:
        explicit Application(const int32_t argc, char** argv) : m_Args(argc, argv){};
        virtual ~Application() = default;

        void Run();
        void Exit();

        static Application& GetCurrentApplication();

        virtual ApplicationConfiguration GetConfiguration() const = 0;

        virtual void OnInit(){}
        virtual void OnUpdate(float deltaTime){}
        virtual void OnPreRender(CommandBuffer& cmdBuffer){}
        virtual void OnRender(CommandBuffer& cmdBuffer){}
        virtual void OnPostRender(CommandBuffer& cmdBuffer){}
        virtual void OnGUI(){}
        virtual void OnDrawDebug(){}
        virtual void OnDestroy(){}

        virtual RenderDeviceType GetRenderDeviceType() const { return RenderDeviceType::Vulkan; }

        float GetDeltaTime() const;
        const Ref<Window>& GetWindow() const;
        Ref<Window>& GetWindow();
        const Ref<RenderDevice>& GetRenderDevice() const;
        Ref<RenderDevice>& GetRenderDevice();
        const Ref<ImGuiRenderer>& GetImGuiRenderer() const;
        SceneManager* GetSceneManager();

        const Ref<RenderTarget>& GetRenderTarget() const;
        Ref<RenderTarget>& GetRenderTarget();

        const AssetDatabase& GetAssetDatabase() const;
        AssetDatabase& GetAssetDatabase();
        slang::IGlobalSession* GetSlangSession() const;

        uint32_t GetWindowWidth() const;
        uint32_t GetWindowHeight() const;

        const CmdLineArgs& GetProgramArguments() { return m_Args; }

        template <typename T>
        Ref<EditorWindow> GetEditorWindow()
        {
            for (Ref<EditorWindow> editorWindow : m_EditorWindows)
            {
                if (Ref<T> window = editorWindow.As<T>())
                    return window;
            }
            return nullptr;
        }

    protected:
        void Update();
        void Render();
        void Destroy();

    private:
        CmdLineArgs m_Args;
        Ref<Window> m_Window = nullptr;
        Ref<RenderDevice> m_Device = nullptr;
        Ref<AudioSystem> m_AudioSystem = nullptr;
        slang::IGlobalSession* m_SlangSession = nullptr;

        Ref<RenderTarget> m_RenderTarget = nullptr;
        Ref<ImGuiRenderer> m_ImGuiRenderer = nullptr;

        SceneManager m_SceneManager;
        AssetDatabase m_AssetDatabase;

        Array<Ref<EditorWindow>> m_EditorWindows;

        bool m_IsRunning = true;
        double m_LastTime = 0.0f;
        double m_DeltaTime = 0.0f;
    };
}
