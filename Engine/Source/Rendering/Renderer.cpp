#include "Renderer.h"
#include "Platform/PlatformRenderer.h"
#include "Platform/D3D12/D3D12IndexBuffer.h"
#include "Platform/D3D12/D3D12Pipeline.h"
#include "Platform/D3D12/D3D12Shader.h"
#include "Platform/D3D12/D3D12VertexBuffer.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Platform/OpenGL/OpenGLRenderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/Vulkan/VulkanIndexBuffer.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanVertexBuffer.h"

namespace Nova
{
    Renderer* Renderer::Create(Application* Owner, const GraphicsApi& GraphicsApi)
    {
        switch (GraphicsApi)
        {
        case GraphicsApi::None:     return nullptr;
        case GraphicsApi::Vulkan:   return new VulkanRenderer(Owner);
        case GraphicsApi::OpenGL:   return new OpenGLRenderer(Owner);
        case GraphicsApi::D3D12:    return new D3D12Renderer(Owner);
        default:                    return nullptr;
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
        case GraphicsApi::None:     return nullptr;
        case GraphicsApi::OpenGL:   return new OpenGLShader(this, *Name, Filepath);
        case GraphicsApi::Vulkan:   return new VulkanShader(this, *Name, Filepath);
        case GraphicsApi::D3D12:    return new D3D12Shader(this, *Name, Filepath);
        default:                    return nullptr;
        }
    }

    Pipeline* Renderer::CreatePipeline(const PipelineSpecification& Specification)
    {
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None:     return nullptr;
        case GraphicsApi::OpenGL:   return nullptr;
        case GraphicsApi::Vulkan:   return new VulkanPipeline(this, Specification);
        case GraphicsApi::D3D12:    return new D3D12Pipeline(this, Specification);
        default:                    return nullptr;
        }
    }

    VertexBuffer* Renderer::CreateVertexBuffer()
    {
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None:     return nullptr;
        case GraphicsApi::OpenGL:   return new OpenGLVertexBuffer(this);
        case GraphicsApi::Vulkan:   return new VulkanVertexBuffer(this);
        case GraphicsApi::D3D12:    return new D3D12VertexBuffer(this);
        default:                    return nullptr;
        }
    }

    VertexBuffer* Renderer::CreateVertexBuffer(const BufferView<Vertex>& Vertices)
    {
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None:     return nullptr;
        case GraphicsApi::OpenGL:   return new OpenGLVertexBuffer(this, Vertices.Data(), Vertices.Count());
        case GraphicsApi::Vulkan:   return new VulkanVertexBuffer(this, Vertices.Data(), Vertices.Count());
        case GraphicsApi::D3D12:    return new D3D12VertexBuffer(this, Vertices.Data(), Vertices.Count());
        default:                    return nullptr;
        }
    }

    IndexBuffer* Renderer::CreateIndexBuffer()
    {
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None:     return nullptr;
        case GraphicsApi::OpenGL:   return new OpenGLIndexBuffer(this);
        case GraphicsApi::Vulkan:   return new VulkanIndexBuffer(this);
        case GraphicsApi::D3D12:    return new D3D12IndexBuffer(this);
        default:                    return nullptr;
        }
    }

    IndexBuffer* Renderer::CreateIndexBuffer(const BufferView<u32>& Indices)
    {
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None:     return nullptr;
        case GraphicsApi::OpenGL:   return new OpenGLIndexBuffer(this, Indices.Data(), Indices.Count());
        case GraphicsApi::Vulkan:   return new VulkanIndexBuffer(this, Indices.Data(), Indices.Count());
        case GraphicsApi::D3D12:    return new D3D12IndexBuffer(this, Indices.Data(), Indices.Count());
        default:                    return nullptr;
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

