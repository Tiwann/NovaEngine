#pragma once
#include "Containers/String.h"
#include "Rendering/Device.h"
#include "Rendering/RenderPass.h"
#include "Rendering/RenderTarget.h"
#include "Rendering/ImGuiRenderer.h"
#include "SceneManager.h"
#include "Window.h"
#include "Ref.h"
#include "AssetDatabase.h"
#include "CmdLineArgs.h"

#include <cstdint>

namespace slang { class IGlobalSession; }

namespace Nova
{
    class Window;
    class AudioSystem;
    class EditorWindow;
    class Event;

    struct ApplicationConfiguration
    {
        String applicationName;
        uint32_t windowWidth, windowHeight;
        WindowCreateFlags windowFlags = WindowCreateFlagBits::Default;
        bool vsync;
    };

    class Application
    {
    public:
        explicit Application(CmdLineArgs&& cmdLineArgs) : m_Args(std::move(cmdLineArgs)){}
        virtual ~Application() = default;

        void Run();
        void Exit();

        static Application& GetCurrentApplication();

        virtual ApplicationConfiguration GetConfiguration() const = 0;
        virtual void OnInit() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnPreRender(CommandBuffer& cmdBuffer) {}
        virtual void OnRender(CommandBuffer& cmdBuffer) {}
        virtual void OnPostRender(CommandBuffer& cmdBuffer) {}
        virtual void OnGUI() {}
        virtual void OnDrawDebug() {}
        virtual void OnDestroy() {}

        float GetDeltaTime() const;
        const Ref<Window>& GetWindow() const;
        Ref<Window>& GetWindow();
        const Ref<Device>& GetDevice() const;
        Ref<Device>& GetDevice();
        const Ref<ImGuiRenderer>& GetImGuiRenderer() const;
        SceneManager* GetSceneManager();

        RenderPass* GetRenderPass();
        const Ref<RenderTarget>& GetRenderTarget() const;

        const AssetDatabase& GetAssetDatabase() const;
        AssetDatabase& GetAssetDatabase();
        slang::IGlobalSession* GetSlangSession() const;

        uint32_t GetWindowWidth() const { return m_Window->GetWidth(); }
        uint32_t GetWindowHeight() const { return m_Window->GetHeight(); }

        template<typename T> requires std::is_base_of_v<Asset, T>
        Ref<T> CreateAsset(const String& name)
        {
            Ref<T> asset = m_AssetDatabase.CreateAsset<T>(name);
            return asset;
        }

        const CmdLineArgs& GetProgramArguments() { return m_Args; }
    protected:
        void Update();
        void Render();
        void Destroy();
    private:
        CmdLineArgs m_Args;
        Ref<Window> m_Window = nullptr;
        Ref<Device> m_Device = nullptr;
        Ref<AudioSystem> m_AudioSystem = nullptr;
        slang::IGlobalSession* m_SlangSession = nullptr;

        Ref<RenderTarget> m_RenderTarget = nullptr;
        RenderPass m_RenderPass;

        Ref<ImGuiRenderer> m_ImGuiRenderer = nullptr;
        RenderPass m_ImGuiRenderPass;

        SceneManager m_SceneManager;
        AssetDatabase m_AssetDatabase;

        Array<Ref<EditorWindow>> m_EditorWindows;

        bool m_IsRunning = true;
        double m_LastTime = 0.0f;
        double m_DeltaTime = 0.0f;
    };
}
