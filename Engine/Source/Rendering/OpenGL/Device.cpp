#include "Device.h"
#include "Runtime/DesktopWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace Nova::OpenGL
{
    void OpenGLErrorCallback(const GLenum source, GLenum, GLuint, const GLenum severity, GLsizei, const GLchar* message, const void *)
    {

    };

    bool Device::Initialize(const DeviceCreateInfo& createInfo)
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

    void Device::Destroy()
    {
    }

    bool Device::BeginFrame()
    {
        return false;
    }

    void Device::EndFrame()
    {
    }

    void Device::Present()
    {
    }

    void Device::WaitIdle() const
    {

    }

    void Device::SetName(StringView name)
    {
    }


    DeviceType Device::GetDeviceType()
    {
        return DeviceType::OpenGL;
    }

    Ref<Nova::RenderTarget> Device::CreateRenderTarget(const RenderTargetCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Surface> Device::CreateSurface(const SurfaceCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Texture> Device::CreateTexture(const TextureCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Texture> Device::CreateTextureUnitialized()
    {
        return nullptr;
    }

    Ref<Nova::Sampler> Device::CreateSampler(const SamplerCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Buffer> Device::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Shader> Device::CreateShader(const ShaderCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::GraphicsPipeline> Device::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::ComputePipeline> Device::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Material> Device::CreateMaterial(const MaterialCreateInfo& createInfo)
    {
        return nullptr;
    }

    Ref<Nova::Fence> Device::CreateFence(const FenceCreateInfo& createInfo)
    {
        return nullptr;
    }

    uint32_t Device::GetImageCount() const
    {
        return 0;
    }
}
