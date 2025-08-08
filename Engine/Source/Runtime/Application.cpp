#include "Application.h"
#include "Time.h"
#include "Window.h"
#include "Rendering/Renderer2D.h"

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

        m_Window = Window::Create(windowCreateInfo);
        if (!m_Window) return;

        m_Window->closeEvent.BindMember(this, &Application::Exit);

        // Creating render device;
        Rendering::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.applicationName = "Hello Triangle";
        deviceCreateInfo.versionMajor = 1;
        deviceCreateInfo.versionMinor = 0;
        deviceCreateInfo.window = m_Window;
        deviceCreateInfo.buffering = SwapchainBuffering::DoubleBuffering;
        deviceCreateInfo.vSync = false;
        if (!m_Device.Initialize(deviceCreateInfo))
            return;

        // Creating render target
        Rendering::RenderTargetCreateInfo renderTargetCreateInfo;
        renderTargetCreateInfo.device = GetDevice();
        renderTargetCreateInfo.width = m_Window->GetWidth();
        renderTargetCreateInfo.height = m_Window->GetHeight();
        renderTargetCreateInfo.depth = 1;
        renderTargetCreateInfo.colorFormat = Format::R8G8B8A8_UNORM;
        renderTargetCreateInfo.depthFormat = Format::D32_FLOAT_S8_UINT;
        renderTargetCreateInfo.sampleCount = 8;
        m_RenderTarget.Initialize(renderTargetCreateInfo);

        // Render target render pass
        {
            Rendering::RenderPassAttachment colorAttachment;
            colorAttachment.type = Rendering::AttachmentType::Color;
            colorAttachment.loadOp = Rendering::LoadOperation::Clear;
            colorAttachment.storeOp = Rendering::StoreOperation::Store;
            colorAttachment.clearValue.color = Color::Black;
            colorAttachment.resolveMode = ResolveMode::Average;

            Rendering::RenderPassAttachment depthAttachment;
            depthAttachment.type = Rendering::AttachmentType::Depth;
            depthAttachment.loadOp = Rendering::LoadOperation::Clear;
            depthAttachment.storeOp = Rendering::StoreOperation::Store;
            depthAttachment.clearValue.depth = 1.0f;
            depthAttachment.clearValue.stencil = 0;
            depthAttachment.resolveMode = ResolveMode::Average;

            m_RenderPass.AddAttachment(colorAttachment);
            m_RenderPass.AddAttachment(depthAttachment);
            m_RenderPass.SetAttachmentTexture(0, m_RenderTarget.GetColorTexture());
            m_RenderPass.SetAttachmentTexture(1, m_RenderTarget.GetDepthTexture());
            m_RenderPass.Initialize(0, 0, m_RenderTarget.GetWidth(), m_RenderTarget.GetHeight());
        }

        // Creating imgui renderer
        Rendering::ImGuiRendererCreateInfo imguiCreateInfo;
        imguiCreateInfo.device = &m_Device;
        imguiCreateInfo.window = m_Window;
        imguiCreateInfo.sampleCount = 1;
        m_ImGuiRenderer.Initialize(imguiCreateInfo);

        // Setup imgui render pass
        Rendering::RenderPassAttachment colorAttachment;
        colorAttachment.type = Rendering::AttachmentType::Color;
        colorAttachment.loadOp = Rendering::LoadOperation::Load;
        colorAttachment.storeOp = Rendering::StoreOperation::Store;
        m_ImGuiRenderPass.Initialize(0, 0, m_Window->GetWidth(), m_Window->GetHeight());
        m_ImGuiRenderPass.AddAttachment(colorAttachment);

        m_Window->maximizeEvent.Bind([this]
        {
            Vulkan::Swapchain* swapchain = m_Device.GetSwapchain();
            swapchain->Invalidate();
        });

        m_Window->resizeEvent.Bind([this](const int32_t newWidth, const int32_t newHeight)
        {
            Vulkan::Swapchain* swapchain = m_Device.GetSwapchain();
            swapchain->Invalidate();
            m_RenderTarget.Resize(newWidth, newHeight);
            m_RenderPass.Resize(newWidth, newHeight);
            m_ImGuiRenderPass.Resize(newWidth, newHeight);
        });

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
        if (m_Device.BeginFrame())
        {
            Vulkan::Swapchain* swapchain = m_Device.GetSwapchain();

            Vulkan::CommandBuffer& cmdBuffer = m_Device.GetCurrentCommandBuffer();
            m_RenderPass.SetAttachmentTexture(0, m_RenderTarget.GetColorTexture());
            m_RenderPass.SetAttachmentTexture(1, m_RenderTarget.GetDepthTexture());
            m_RenderPass.SetAttachmentResolveTexture(0, swapchain->GetCurrentTexture());


            cmdBuffer.BeginRenderPass(m_RenderPass);
            m_SceneManager.OnRender(cmdBuffer);
            OnRender(cmdBuffer);
            cmdBuffer.EndRenderPass();

            m_ImGuiRenderer.BeginFrame();
            OnGUI();
            m_ImGuiRenderer.EndFrame();


            m_ImGuiRenderPass.SetAttachmentTexture(0, swapchain->GetCurrentTexture());
            cmdBuffer.BeginRenderPass(m_ImGuiRenderPass);
            m_ImGuiRenderer.Render(cmdBuffer);
            cmdBuffer.EndRenderPass();

            m_Device.EndFrame();
            m_Device.Present();
        }
    }

    void Application::Destroy()
    {
        OnDestroy();
        m_Device.WaitIdle();
        m_ImGuiRenderer.Destroy();
        m_Device.Destroy();
        m_Window->Destroy();
    }

    float Application::GetDeltaTime() const
    {
        return m_DeltaTime;
    }

    Window* Application::GetWindow() const
    {
        return m_Window;
    }

    Vulkan::Device* Application::GetDevice()
    {
        return &m_Device;
    }

    Vulkan::ImGuiRenderer* Application::GetImGuiRenderer()
    {
        return &m_ImGuiRenderer;
    }

    SceneManager* Application::GetSceneManager()
    {
        return &m_SceneManager;
    }

    Rendering::RenderPass* Application::GetRenderPass()
    {
        return &m_RenderPass;
    }

    Vulkan::RenderTarget* Application::GetRenderTarget()
    {
        return &m_RenderTarget;
    }
}
