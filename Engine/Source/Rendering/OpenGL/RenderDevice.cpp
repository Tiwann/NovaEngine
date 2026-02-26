#include "RenderDevice.h"
#include "Runtime/DesktopWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace Nova::OpenGL
{
    void OpenGLErrorCallback(const GLenum source, GLenum, GLuint, const GLenum severity, GLsizei, const GLchar* message, const void *)
    {

    };

    bool RenderDevice::Initialize(const DeviceCreateInfo& createInfo)
    {
        if (DesktopWindow* window = dynamic_cast<DesktopWindow*>(createInfo.window))
        {
            glfwMakeContextCurrent(window->GetHandle());
        }

        glfwSwapInterval(createInfo.vSync);
        
        if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            std::println(std::cerr, "Failed to retrieve OpenGL function pointers!");
            return false;
        }

#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(OpenGLErrorCallback, nullptr);
#endif

        return true;
    }

    void RenderDevice::Destroy()
    {
    }

    bool RenderDevice::BeginFrame()
    {
        return false;
    }

    void RenderDevice::EndFrame()
    {
    }

    void RenderDevice::Present()
    {
    }

    void RenderDevice::WaitIdle() const
    {

    }

    void RenderDevice::SetName(StringView name)
    {
    }


    DeviceType RenderDevice::GetDeviceType()
    {
        return DeviceType::OpenGL;
    }

    Ref<Nova::RenderTarget> RenderDevice::CreateRenderTarget(const RenderTargetCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Surface> RenderDevice::CreateSurface(const SurfaceCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Texture> RenderDevice::CreateTexture(const TextureCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Texture> RenderDevice::CreateTextureUnitialized()
    {
        return nullptr;
    }

    Ref<Nova::Sampler> RenderDevice::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Buffer> RenderDevice::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Shader> RenderDevice::CreateShader(const ShaderCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::GraphicsPipeline> RenderDevice::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
    {
        return nullptr;
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
        return 0;
    }
}
