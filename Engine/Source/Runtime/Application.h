#pragma once
#include "Containers/String.h"
#include "Rendering/Vulkan/Device.h"
#include "Rendering/Vulkan/ImGuiRenderer.h"
#include "Game/SceneManager.h"
#include "Rendering/RenderPass.h"
#include "Rendering/Vulkan/RenderTarget.h"

#include <cstdint>

namespace Nova
{
    class Window;

    struct ApplicationCreateInfo
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


        virtual void OnInit() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender(Vulkan::CommandBuffer& cmdBuffer) {}
        virtual void OnGUI() {}
        virtual void OnDestroy() {}

        float GetDeltaTime() const;
        Window* GetWindow() const;
        Vulkan::Device* GetDevice();
        Vulkan::ImGuiRenderer* GetImGuiRenderer();
        SceneManager* GetSceneManager();

        Rendering::RenderPass* GetRenderPass();
        Vulkan::RenderTarget* GetRenderTarget();
    protected:
        void Update();
        void Render();
        void Destroy();
    private:
        Window* m_Window = nullptr;
        Vulkan::Device m_Device;
        Vulkan::RenderTarget m_RenderTarget;
        Rendering::RenderPass m_RenderPass;
        Vulkan::ImGuiRenderer m_ImGuiRenderer;
        Rendering::RenderPass m_ImGuiRenderPass;

        SceneManager m_SceneManager;
        bool m_IsRunning = true;

        double m_LastTime = 0.0f;
        double m_DeltaTime = 0.0f;
    };
}
