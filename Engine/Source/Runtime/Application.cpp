#include "Application.h"

#include "Path.h"
#include "Time.h"
#include "Window.h"
#include "Rendering/Shader.h"
#include "Rendering/Vulkan/Device.h"
#include "Rendering/Vulkan/RenderTarget.h"
#include "Rendering/Vulkan/Shader.h"
#include "Rendering/Vulkan/Swapchain.h"

namespace Nova
{
    void Application::Run()
    {
        // Creating window
        WindowCreateInfo windowCreateInfo;
        windowCreateInfo.title = "Hello Triangle";
        windowCreateInfo.width = 600;
        windowCreateInfo.height = 400;
        windowCreateInfo.resizable = true;
        windowCreateInfo.show = true;
        m_Window = CreateWindow(windowCreateInfo);
        if (!m_Window) return;

        m_Window->closeEvent.BindMember(this, &Application::Exit);

        // Creating render device;
        Rendering::DeviceCreateInfo rdCreateInfo;
        rdCreateInfo.applicationName = "Hello Triangle";
        rdCreateInfo.versionMajor = 1;
        rdCreateInfo.versionMinor = 0;
        rdCreateInfo.window = m_Window;
        rdCreateInfo.buffering = SwapchainBuffering::DoubleBuffering;
        rdCreateInfo.vSync = false;
        m_Device = CreateRenderDevice(Rendering::DeviceType::Vulkan, rdCreateInfo);
        if (!m_Device) return;


        // Creating render target
        Rendering::RenderTargetCreateInfo rtCreateInfo;
        rtCreateInfo.device = GetDevice();
        rtCreateInfo.width = m_Window->GetWidth();
        rtCreateInfo.height = m_Window->GetHeight();
        rtCreateInfo.depth = 1;
        rtCreateInfo.colorFormat = Format::R8G8B8A8_UNORM;
        rtCreateInfo.depthFormat = Format::D32_FLOAT_S8_UINT;
        rtCreateInfo.sampleCount = 8;
        m_RenderTarget = CreateRenderTarget(rtCreateInfo);
        if (!m_RenderTarget) return;

        // Render target render pass

        {
            Rendering::RenderPassAttachment colorAttachment;
            colorAttachment.type = Rendering::AttachmentType::Color;
            colorAttachment.loadOp = Rendering::LoadOperation::Clear;
            colorAttachment.storeOp = Rendering::StoreOperation::Store;
            colorAttachment.clearValue.color = Color::Black;
            colorAttachment.resolveMode = ResolveMode::Average;
            m_RenderPass.AddAttachment(colorAttachment);

            Rendering::RenderPassAttachment depthAttachment;
            depthAttachment.type = Rendering::AttachmentType::Depth;
            depthAttachment.loadOp = Rendering::LoadOperation::Clear;
            depthAttachment.storeOp = Rendering::StoreOperation::Store;
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
        Rendering::RenderPassAttachment colorAttachment;
        colorAttachment.type = Rendering::AttachmentType::Color;
        colorAttachment.loadOp = Rendering::LoadOperation::Load;
        colorAttachment.storeOp = Rendering::StoreOperation::Store;
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


        // Load engine shaders
        const auto loadShader = [this](const String& shaderName, const String& shaderPath)
        {
            Rendering::ShaderCreateInfo spriteShaderCreateInfo;
            spriteShaderCreateInfo.device = m_Device;
            spriteShaderCreateInfo.slang = m_SlangSession;
            spriteShaderCreateInfo.target = Rendering::ShaderTarget::SPIRV;
            spriteShaderCreateInfo.entryPoints.Add({ "vert", ShaderStageFlagBits::Vertex });
            spriteShaderCreateInfo.entryPoints.Add({ "frag", ShaderStageFlagBits::Fragment });
            spriteShaderCreateInfo.moduleInfo = { shaderName, shaderPath };

            Rendering::Shader* spriteShader = m_AssetDatabase.CreateAsset<Vulkan::Shader>("Sprite");
            spriteShader->SetObjectName(shaderName);
            spriteShader->Initialize(spriteShaderCreateInfo);
        };
        loadShader("Sprite", Path::GetEngineAssetPath("Shaders/Sprite.slang"));

        OnInit();
        Update();
    }

    void Application::Exit()
    {
        m_IsRunning = false;
    }

    void Application::Update()
    {
        while (m_IsRunning)
        {
            const double currentTime = Time::Get();
            m_DeltaTime = currentTime - m_LastTime;
            m_LastTime = currentTime;
            m_Window->PollEvents();

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
            OnPreRender(cmdBuffer);

            Vulkan::Swapchain* swapchain = m_Device.As<Vulkan::Device>()->GetSwapchain();
            m_RenderPass.SetAttachmentTexture(0, m_RenderTarget.As<Vulkan::RenderTarget>()->GetColorTexture());
            m_RenderPass.SetAttachmentTexture(1, m_RenderTarget.As<Vulkan::RenderTarget>()->GetDepthTexture());
            m_RenderPass.SetAttachmentResolveTexture(0, swapchain->GetCurrentTexture());

            cmdBuffer.BeginRenderPass(m_RenderPass);
            m_SceneManager.OnRender(cmdBuffer);
            OnRender(cmdBuffer);
            cmdBuffer.EndRenderPass();

            m_ImGuiRenderer->BeginFrame();
            OnGUI();
            m_ImGuiRenderer->EndFrame();


            m_ImGuiRenderPass.SetAttachmentTexture(0, swapchain->GetCurrentTexture());
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

    const Ref<Rendering::Device>& Application::GetDevice() const
    {
        return m_Device;
    }

    const Ref<Rendering::ImGuiRenderer>& Application::GetImGuiRenderer() const
    {
        return m_ImGuiRenderer;
    }

    SceneManager* Application::GetSceneManager()
    {
        return &m_SceneManager;
    }

    Rendering::RenderPass* Application::GetRenderPass()
    {
        return &m_RenderPass;
    }

    const Ref<Rendering::RenderTarget>& Application::GetRenderTarget() const
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
