#include "Application.h"
#include "Path.h"
#include "Scene.h"
#include "Time.h"
#include "Window.h"
#include "Audio/AudioSystem.h"
#include "Components/Camera.h"
#include "Editor/HierarchyWindow.h"
#include "Editor/InspectorWindow.h"
#include "Rendering/DebugRenderer.h"
#include "Rendering/Shader.h"
#include "Rendering/Vulkan/Device.h"
#include "Rendering/Vulkan/RenderTarget.h"
#include "Rendering/Vulkan/Shader.h"
#include "Rendering/Vulkan/Swapchain.h"

#include <imgui.h>

#include "Utils/TextureUtils.h"

namespace Nova
{
    extern Application* g_Application;

    void Application::Run()
    {
        ApplicationConfiguration configuration = GetConfiguration();

        // Creating window
        WindowCreateInfo windowCreateInfo;
        windowCreateInfo.title = configuration.applicationName;
        windowCreateInfo.width = configuration.windowWidth;
        windowCreateInfo.height = configuration.windowHeight;
        windowCreateInfo.flags = configuration.windowFlags;
        m_Window = CreateWindow(windowCreateInfo);
        if (!m_Window) return;

        m_Window->closeEvent.BindMember(this, &Application::Exit);

        // Creating render device;
        DeviceCreateInfo rdCreateInfo;
        rdCreateInfo.appName = configuration.applicationName;
        rdCreateInfo.window = m_Window;
        rdCreateInfo.buffering = SwapchainBuffering::DoubleBuffering;
        rdCreateInfo.vSync = configuration.vsync;
        m_Device = CreateRenderDevice(DeviceType::Vulkan, rdCreateInfo);
        if (!m_Device) return;


        // Creating render target
        RenderTargetCreateInfo rtCreateInfo;
        rtCreateInfo.device = GetDevice();
        rtCreateInfo.width = m_Window->GetWidth();
        rtCreateInfo.height = m_Window->GetHeight();
        rtCreateInfo.depth = 1;
        rtCreateInfo.colorFormat = Format::R8G8B8A8_SRGB;
        rtCreateInfo.depthFormat = Format::D32_FLOAT_S8_UINT;
        rtCreateInfo.sampleCount = 8;
        m_RenderTarget = CreateRenderTarget(rtCreateInfo);
        if (!m_RenderTarget) return;

        // Render target render pass
        {
            RenderPassAttachment colorAttachment;
            colorAttachment.type = AttachmentType::Color;
            colorAttachment.loadOp = LoadOperation::Clear;
            colorAttachment.storeOp = StoreOperation::Store;
            colorAttachment.clearValue.color = Color::Black;
            colorAttachment.resolveMode = ResolveMode::Average;
            m_RenderPass.AddAttachment(colorAttachment);

            RenderPassAttachment depthAttachment;
            depthAttachment.type = AttachmentType::Depth;
            depthAttachment.loadOp = LoadOperation::Clear;
            depthAttachment.storeOp = StoreOperation::Store;
            depthAttachment.clearValue.depth = 1.0f;
            depthAttachment.clearValue.stencil = 0;
            depthAttachment.resolveMode = ResolveMode::Average;
            m_RenderPass.AddAttachment(depthAttachment);

            m_RenderPass.SetAttachmentTexture(0, m_RenderTarget.As<Vulkan::RenderTarget>()->GetColorTexture());
            m_RenderPass.SetAttachmentTexture(1, m_RenderTarget.As<Vulkan::RenderTarget>()->GetDepthTexture());
            m_RenderPass.Initialize(0, 0, m_RenderTarget->GetWidth(), m_RenderTarget->GetHeight());
        }

        // Creating imgui renderer
        m_ImGuiRenderer = CreateImGuiRenderer(m_Window, m_Device, 1);

        // Setup imgui render pass
        RenderPassAttachment colorAttachment;
        colorAttachment.type = AttachmentType::Color;
        colorAttachment.loadOp = LoadOperation::Load;
        colorAttachment.storeOp = StoreOperation::Store;
        m_ImGuiRenderPass.Initialize(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
        m_ImGuiRenderPass.AddAttachment(colorAttachment);

        m_Window->maximizeEvent.Bind([this]
        {
            if (Vulkan::Swapchain* swapchain = m_Device.As<Vulkan::Device>()->GetSwapchain())
                swapchain->Invalidate();
        });

        m_Window->resizeEvent.Bind([this](const int32_t newWidth, const int32_t newHeight)
        {
            if (Vulkan::Swapchain* swapchain = m_Device.As<Vulkan::Device>()->GetSwapchain())
                swapchain->Invalidate();
            m_RenderTarget->Resize(newWidth, newHeight);
            m_RenderPass.Resize(newWidth, newHeight);
            m_ImGuiRenderPass.Resize(newWidth, newHeight);
        });

        if (SLANG_FAILED(slang::createGlobalSession(&m_SlangSession)))
            return;

        AudioSystemCreateInfo audioSystemCreateInfo;
        audioSystemCreateInfo.channels = 2;
        audioSystemCreateInfo.sampleRate = 44100;
        audioSystemCreateInfo.listenerCount = 1;
        m_AudioSystem = CreateAudioSystem(audioSystemCreateInfo);


        // Load engine shaders
        const auto LoadShaderBasic = [this](const String& moduleName, const String& shaderName, const String& shaderPath) -> Ref<Shader>
        {
            const ShaderEntryPoint entryPoints []
            {
                { "vert", ShaderStageFlagBits::Vertex },
                { "frag", ShaderStageFlagBits::Fragment }
            };

            ShaderCreateInfo shaderCreateInfo;
            shaderCreateInfo.slang = m_SlangSession;
            shaderCreateInfo.target = ShaderTarget::SPIRV;
            shaderCreateInfo.entryPoints.AddRange(entryPoints);
            shaderCreateInfo.moduleInfo = { moduleName, shaderPath };

            Ref<Shader> shader = m_Device->CreateShader(shaderCreateInfo);
            if (!shader) return nullptr;

            shader->SetObjectName(shaderName);
            m_AssetDatabase.AddAsset(shader, shaderName);
            return shader;
        };

        const auto LoadTextureBasic = [this](StringView filepath, const String& assetName)
        {
            Ref<Texture> texture = LoadTexture(GetDevice(), filepath);
            m_AssetDatabase.AddAsset(texture, assetName);
            return texture;
        };

        LoadShaderBasic("Sprite", "SpriteShader", Path::GetEngineAssetPath("Shaders/Sprite.slang"));
        LoadShaderBasic("BlinnPhong", "BlinnPhongShader", Path::GetEngineAssetPath("Shaders/BlinnPhong.slang"));
        LoadShaderBasic("Fullscreen", "FullscreenShader", Path::GetEngineAssetPath("Shaders/Fullscreen.slang"));
        Ref<Shader> debugShader = LoadShaderBasic("Debug", "DebugShader", Path::GetEngineAssetPath("Shaders/Debug.slang"));

        LoadTextureBasic(Path::GetEngineAssetPath("Textures/BlackTexPlaceholder.png"), "BlackTexPlaceholder");
        LoadTextureBasic(Path::GetEngineAssetPath("Textures/WhiteTexPlaceholder.png"), "WhiteTexPlaceholder");
        LoadTextureBasic(Path::GetEngineAssetPath("Textures/GreyTexPlaceholder.png"), "GreyTexPlaceholder");
        LoadTextureBasic(Path::GetEngineAssetPath("Textures/CheckerTexPlaceholder.png"), "CheckerTexPlaceholder");
        LoadTextureBasic(Path::GetEngineAssetPath("Textures/NormalTexPlaceholder.png"), "NormalTexPlaceholder");

        DebugRendererCreateInfo debugRendererCreateInfo;
        debugRendererCreateInfo.device = m_Device;
        debugRendererCreateInfo.shader = debugShader;
        debugRendererCreateInfo.renderPass = &m_RenderPass;
        debugRendererCreateInfo.maxVertices = 64;
        if (!DebugRenderer::Initialize(debugRendererCreateInfo))
            return;


        m_EditorWindows.Add(EditorWindow::CreateWindow<HierarchyWindow>());
        m_EditorWindows.Add(EditorWindow::CreateWindow<InspectorWindow>());

        OnInit();
        Update();
    }

    void Application::Exit()
    {
        m_IsRunning = false;
    }

    Application& Application::GetCurrentApplication()
    {
        return *g_Application;
    }

    void Application::Update()
    {
        while (m_IsRunning)
        {
            const double currentTime = Time::Get();
            m_DeltaTime = currentTime - m_LastTime;
            m_LastTime = currentTime;
            m_Window->PollEvents();

            for (Ref<EditorWindow>& window : m_EditorWindows)
                window->OnUpdate(m_DeltaTime);

            m_SceneManager.OnUpdate(m_DeltaTime);
            OnUpdate(m_DeltaTime);

            Render();
        }
    }

    void Application::Render()
    {
        if (m_Device->BeginFrame())
        {
            Vulkan::CommandBuffer& cmdBuffer = m_Device.As<Vulkan::Device>()->GetCurrentCommandBuffer();
            m_SceneManager.OnPreRender(cmdBuffer);
            OnPreRender(cmdBuffer);

            Vulkan::Swapchain* swapchain = m_Device.As<Vulkan::Device>()->GetSwapchain();
            m_RenderPass.SetAttachmentTexture(0, m_RenderTarget.As<Vulkan::RenderTarget>()->GetColorTexture());
            m_RenderPass.SetAttachmentTexture(1, m_RenderTarget.As<Vulkan::RenderTarget>()->GetDepthTexture());
            m_RenderPass.SetAttachmentResolveTexture(0, *swapchain->GetCurrentTexture());

            if (Scene* scene = m_SceneManager.GetActiveScene())
            {
                if (Camera* camera = scene->GetFirstComponent<Camera>())
                {
                    DebugRenderer::Begin(camera->GetViewProjectionMatrix());
                    m_SceneManager.OnDrawDebug();
                    OnDrawDebug();
                    DebugRenderer::End(cmdBuffer);
                }
            }


            cmdBuffer.BeginRenderPass(m_RenderPass);
            m_SceneManager.OnRender(cmdBuffer);
            OnRender(cmdBuffer);
            DebugRenderer::Render(cmdBuffer);
            cmdBuffer.EndRenderPass();

            m_ImGuiRenderer->BeginFrame();
            ImGui::DockSpaceOverViewport(ImGui::GetID("Dockspace"), ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
            for (Ref<EditorWindow>& window : m_EditorWindows)
                window->OnGui();
            OnGUI();
            m_ImGuiRenderer->EndFrame();


            m_ImGuiRenderPass.SetAttachmentTexture(0, *swapchain->GetCurrentTexture());
            cmdBuffer.BeginRenderPass(m_ImGuiRenderPass);
            m_ImGuiRenderer->Render(cmdBuffer);
            cmdBuffer.EndRenderPass();

            m_Device->EndFrame();
            m_Device->Present();
        }
    }

    void Application::Destroy()
    {
        m_Device->WaitIdle();
        OnDestroy();
        DebugRenderer::Destroy();
        m_AssetDatabase.UnloadAll();
        m_SlangSession->release();
        slang::shutdown();
        m_RenderTarget->Destroy();
        m_ImGuiRenderer->Destroy();
        m_Device->Destroy();
        m_Window->Destroy();
    }

    float Application::GetDeltaTime() const
    {
        return m_DeltaTime;
    }

    const Ref<Window>& Application::GetWindow() const
    {
        return m_Window;
    }

    Ref<Window>& Application::GetWindow()
    {
        return m_Window;
    }

    const Ref<Device>& Application::GetDevice() const
    {
        return m_Device;
    }

    Ref<Device>& Application::GetDevice()
    {
        return m_Device;
    }

    const Ref<ImGuiRenderer>& Application::GetImGuiRenderer() const
    {
        return m_ImGuiRenderer;
    }

    SceneManager* Application::GetSceneManager()
    {
        return &m_SceneManager;
    }

    RenderPass* Application::GetRenderPass()
    {
        return &m_RenderPass;
    }

    const Ref<RenderTarget>& Application::GetRenderTarget() const
    {
        return m_RenderTarget;
    }

    const AssetDatabase& Application::GetAssetDatabase() const
    {
        return m_AssetDatabase;
    }

    AssetDatabase& Application::GetAssetDatabase()
    {
        return m_AssetDatabase;
    }

    slang::IGlobalSession* Application::GetSlangSession() const
    {
        return m_SlangSession;
    }
}
