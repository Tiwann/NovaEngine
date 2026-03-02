#include "RenderDevice.h"
#include "Runtime/DesktopWindow.h"
#include "Runtime/Log.h"
#include "Shader.h"
#include "Sampler.h"
#include "Buffer.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "GraphicsPipeline.h"
#include "Rendering/Surface.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace Nova::OpenGL
{
#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
    void static OpenGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,const GLchar *message, const void *userParam)
    {
        const auto SeverityToVerbosity = [](const GLenum inSeverity)
        {
            switch (inSeverity)
            {
                case GL_DEBUG_SEVERITY_HIGH: return Verbosity::Error;
                case GL_DEBUG_SEVERITY_MEDIUM: return Verbosity::Warning;
                case GL_DEBUG_SEVERITY_LOW: return Verbosity::Trace;
                default: return Verbosity::Trace;
            }
        };

        const StringView messageView(message, length);
        switch (SeverityToVerbosity(severity))
        {
        case Verbosity::Trace:
            NOVA_LOG(RenderDevice, Verbosity::Trace, "OpenGL: {}", messageView);
            break;
        case Verbosity::Info:
            NOVA_LOG(RenderDevice, Verbosity::Info, "OpenGL: {}", messageView);
        case Verbosity::Warning:
            NOVA_LOG(RenderDevice, Verbosity::Warning, "OpenGL Warning {} [{:#06x}]: {}", id, id, messageView);
            break;
        case Verbosity::Error:
            NOVA_LOG(RenderDevice, Verbosity::Error, "OpenGL Error {} [{:#06x}]: {}", id, id, messageView);
            break;
        }
    };
#endif

    bool RenderDevice::Initialize(const DeviceCreateInfo& createInfo)
    {
        if (!createInfo.window)
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to initialize Render device: Invalid window!");
            return false;
        }

        if (DesktopWindow* window = dynamic_cast<DesktopWindow*>(createInfo.window))
        {
            glfwMakeContextCurrent(window->GetHandle());
        }

        glfwSwapInterval(createInfo.vSync);
        
        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to retrieve OpenGL function pointers!");
            return false;
        }

#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(OpenGLErrorCallback, nullptr);
#endif

        glEnable(GL_FRAMEBUFFER_SRGB);

        SurfaceCreateInfo surfaceCreateInfo;
        surfaceCreateInfo.device = this;
        surfaceCreateInfo.window = createInfo.window;
        if (!m_Surface.Initialize(surfaceCreateInfo))
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to initialize surface!");
            return false;
        }

        SwapchainCreateInfo scCreateInfo;
        scCreateInfo.device = this;
        scCreateInfo.surface = &m_Surface;
        scCreateInfo.recycle = false;
        scCreateInfo.buffering = createInfo.buffering;
        scCreateInfo.format = Format::R8G8B8A8_SRGB;
        scCreateInfo.width = m_Surface.GetWidth();
        scCreateInfo.height = m_Surface.GetHeight();
        scCreateInfo.presentMode = createInfo.vSync ? PresentMode::Fifo : PresentMode::Immediate;
        if (!m_Swapchain.Initialize(scCreateInfo))
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to create swapchain!");
            return false;
        }
        return true;
    }

    void RenderDevice::Destroy()
    {
    }

    bool RenderDevice::BeginFrame()
    {
        if (!m_Surface.IsAvailable())
            return false;

        if (!m_Swapchain.IsValid())
        {
            WaitIdle();
            m_Swapchain.Recreate();
            return false;
        }

        if (!m_Swapchain.AcquireNextImage(m_CurrentFrameIndex))
        {
            m_Swapchain.Invalidate();
            return false;
        }

        CommandBuffer& commandBuffer = m_CommandBuffers[m_CurrentFrameIndex];
        const CommandBufferBeginInfo beginInfo { CommandBufferUsageFlagBits::OneTimeSubmit };
        if (!commandBuffer.Begin(beginInfo))
            return false;

        return true;
    }

    void RenderDevice::EndFrame()
    {
        CommandBuffer& commandBuffer = m_CommandBuffers[m_CurrentFrameIndex];
        commandBuffer.End();
        m_GraphicsQueue.Submit(&commandBuffer, nullptr, nullptr, nullptr, 0);
    }

    void RenderDevice::Present()
    {
        (void)m_GraphicsQueue.Present(m_Swapchain, nullptr, m_CurrentFrameIndex);
    }

    void RenderDevice::WaitIdle() const
    {
        glFinish();
    }

    void RenderDevice::SetName(StringView name)
    {
    }


    RenderDeviceType RenderDevice::GetDeviceType()
    {
        return RenderDeviceType::OpenGL;
    }

    Ref<Nova::RenderTarget> RenderDevice::CreateRenderTarget(const RenderTargetCreateInfo& createInfo)
    {
        RenderTarget* renderTarget = new OpenGL::RenderTarget();
        if (!renderTarget->Initialize(createInfo))
        {
            delete renderTarget;
            return nullptr;
        }
        return Ref(renderTarget);
    }

    Ref<Nova::Texture> RenderDevice::CreateTexture(const TextureCreateInfo& createInfo)
    {
        Texture* texture = new Texture();
        TextureCreateInfo textureCreateInfo(createInfo);
        if (!texture->Initialize(textureCreateInfo.WithDevice(this)))
        {
            delete texture;
            return nullptr;
        }
        return Ref(texture);
    }

    Ref<Nova::Texture> RenderDevice::CreateTextureUnitialized()
    {
        return nullptr;
    }

    Ref<Nova::Sampler> RenderDevice::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        Sampler* sampler = new Sampler();
        SamplerCreateInfo samplerCreateInfo(createInfo);
        if (!sampler->Initialize(samplerCreateInfo.WithDevice(this)))
        {
            delete sampler;
            return nullptr;
        }
        return Ref(sampler);
    }

    Ref<Nova::Buffer> RenderDevice::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        Buffer* buffer = new Buffer();
        BufferCreateInfo bufferCreateInfo(createInfo);
        if (!buffer->Initialize(bufferCreateInfo.WithDevice(this)))
        {
            delete buffer;
            return nullptr;
        }
        return Ref(buffer);
    }

    Ref<Nova::Shader> RenderDevice::CreateShader(const ShaderCreateInfo& createInfo)
    {
        Shader* shader = new Shader();
        ShaderCreateInfo shaderCreateInfo(createInfo);
        if (!shader->Initialize(shaderCreateInfo.WithDevice(this)))
        {
            delete shader;
            return nullptr;
        }
        return Ref(shader);
    }

    Ref<Nova::GraphicsPipeline> RenderDevice::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
    {
        GraphicsPipeline* pipeline = new GraphicsPipeline();
        GraphicsPipelineCreateInfo pipelineCreateInfo(createInfo);
        if (!pipeline->Initialize(pipelineCreateInfo.SetDevice(this)))
        {
            delete pipeline;
            return nullptr;
        }
        return Ref(pipeline);
    }

    Ref<Nova::ComputePipeline> RenderDevice::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Material> RenderDevice::CreateMaterial(const MaterialCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Fence> RenderDevice::CreateFence(const FenceCreateInfo& createInfo)
    {
        return nullptr;
    }

    uint32_t RenderDevice::GetImageCount() const
    {
        return m_Swapchain.GetImageCount();
    }

    Nova::Swapchain* RenderDevice::GetSwapchain()
    {
        return &m_Swapchain;
    }

    Nova::CommandBuffer* RenderDevice::GetCurrentCommandBuffer()
    {
        return &m_CommandBuffers[m_CurrentFrameIndex];
    }
}
