#include "Renderer.h"
#include "Platform/PlatformRenderer.h"
#include "Platform/D3D12/D3D12Shader.h"
#include "Platform/OpenGL/OpenGLRenderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace Nova
{
    Renderer* Renderer::Create(Application* Owner, const GraphicsApi& GraphicsApi)
    {
        switch (GraphicsApi)
        {
        case GraphicsApi::None:
            return nullptr;
        case GraphicsApi::Vulkan:
            return new VulkanRenderer(Owner);
        case GraphicsApi::OpenGL:
            return new OpenGLRenderer(Owner);
        case GraphicsApi::D3D12:
            return new D3D12Renderer(Owner);
        default:
            return nullptr;
        }
    }

    void Renderer::Clear(const Color& Color, float Depth)
    {
        ClearColor(Color);
        ClearDepth(Depth);
    }

    Shader* Renderer::CreateShader(const String& Name, const Path& Filepath)
    {
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return new OpenGLShader(this, *Name, Filepath);
        case GraphicsApi::Vulkan: return new VulkanShader(this, *Name, Filepath);
        case GraphicsApi::D3D12: return new D3D12Shader(this, *Name, Filepath);
        default: return nullptr;
        }
    }

    void Renderer::SetCurrentCamera(Camera* Camera)
    {
        m_CurrentCamera = Camera;
    }

    Camera* Renderer::GetCurrentCamera()
    {
        return m_CurrentCamera;
    }

    const Camera* Renderer::GetCurrentCamera() const
    {
        return m_CurrentCamera;
    }

    GraphicsApi Renderer::GetGraphicsApi() const
    {
        return m_GraphicsApi;
    }

    Application* Renderer::GetOwner()
    {
        return m_Application;
    }

    const Application* Renderer::GetOwner() const
    {
        return m_Application;
    }
}

