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

#include <cstdint>

namespace slang { class IGlobalSession; }

namespace Nova
{
    class Window;
    class AudioSystem;

    struct ApplicationConfiguration
    {
        String applicationName;
        uint32_t windowWidth, windowHeight;
        bool resizable;
        bool fullscreen;
        bool vsync;
    };

    class Application
    {
    public:
        Application() = default;
        virtual ~Application() = default;

        void Run();
        void Exit();

        virtual ApplicationConfiguration GetConfiguration() const = 0;
        virtual void OnInit() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnPreRender(Rendering::CommandBuffer& cmdBuffer) {}
        virtual void OnRender(Rendering::CommandBuffer& cmdBuffer) {}
        virtual void OnGUI() {}
        virtual void OnDestroy() {}

        float GetDeltaTime() const;
        const Ref<Window>& GetWindow() const;
        const Ref<Rendering::Device>& GetDevice() const;
        const Ref<Rendering::ImGuiRenderer>& GetImGuiRenderer() const;
        SceneManager* GetSceneManager();

        Rendering::RenderPass* GetRenderPass();
        const Ref<Rendering::RenderTarget>& GetRenderTarget() const;

        const AssetDatabase& GetAssetDatabase() const;
        AssetDatabase& GetAssetDatabase();
        slang::IGlobalSession* GetSlangSession() const;
    protected:
        void Update();
        void Render();
        void Destroy();
    private:
        Ref<Window> m_Window = nullptr;
        Ref<Rendering::Device> m_Device = nullptr;
        Ref<AudioSystem> m_AudioSystem = nullptr;
        slang::IGlobalSession* m_SlangSession = nullptr;

        Ref<Rendering::RenderTarget> m_RenderTarget = nullptr;
        Rendering::RenderPass m_RenderPass;

        Ref<Rendering::ImGuiRenderer> m_ImGuiRenderer = nullptr;
        Rendering::RenderPass m_ImGuiRenderPass;

        SceneManager m_SceneManager;
        AssetDatabase m_AssetDatabase;

        bool m_IsRunning = true;
        double m_LastTime = 0.0f;
        double m_DeltaTime = 0.0f;
    };
}
