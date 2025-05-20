#include "Renderer.h"
#include "Platform/PlatformRenderer.h"
#include "Platform/D3D12/D3D12IndexBuffer.h"
#include "Platform/D3D12/D3D12Pipeline.h"
#include "Platform/D3D12/D3D12Shader.h"
#include "Platform/D3D12/D3D12Swapchain.h"
#include "Platform/D3D12/D3D12VertexBuffer.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Platform/OpenGL/OpenGLPipeline.h"
#include "Platform/OpenGL/OpenGLRenderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/Vulkan/VulkanCommandPool.h"
#include "Platform/Vulkan/VulkanFence.h"
#include "Platform/Vulkan/VulkanIndexBuffer.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Platform/Vulkan/VulkanRenderTarget.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanSwapchain.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"
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

    Fence* Renderer::CreateFence(const FenceCreateInfo& CreateInfo)
    {
        Fence* Result = nullptr;
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return nullptr;
        case GraphicsApi::Vulkan: Result = new VulkanFence(this); break;
        case GraphicsApi::D3D12: return nullptr;
        }

        if (!Result->Initialize(CreateInfo))
        {
            delete Result;
            return nullptr;
        }
        return Result;
    }

    RenderTarget* Renderer::CreateRenderTarget(const RenderTargetCreateInfo& CreateInfo)
    {
        RenderTarget* Result = nullptr;
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return nullptr;
        case GraphicsApi::Vulkan: Result = new VulkanRenderTarget(this); break;
        case GraphicsApi::D3D12: return nullptr;
        }

        if (!Result->Initialize(CreateInfo))
        {
            delete Result;
            return nullptr;
        }
        return Result;
    }

    CommandPool* Renderer::CreateCommandPool(const CommandPoolCreateInfo& CreateInfo)
    {
        CommandPool* Result = nullptr;
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return nullptr;
        case GraphicsApi::Vulkan: Result = new VulkanCommandPool(this); break;
        case GraphicsApi::D3D12: return nullptr;
        default: return nullptr;
        }

        if (!Result->Initialize(CreateInfo))
        {
            delete Result;
            return nullptr;
        }
        return Result;
    }

    Swapchain* Renderer::CreateSwapchain(const SwapchainCreateInfo& CreateInfo)
    {
        Swapchain* Result = nullptr;
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return nullptr;
        case GraphicsApi::Vulkan: Result = new VulkanSwapchain(this); break;
        case GraphicsApi::D3D12: Result = new D3D12Swapchain(this); break;
        default: return nullptr;
        }

        if (!Result->Initialize(CreateInfo))
        {
            delete Result;
            return nullptr;
        }
        return Result;
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
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL:
            {
                Pipeline* Result = new OpenGLPipeline(this);
                if (!Result->Initialize(Specification))
                {
                    delete Result;
                    return nullptr;
                }
                return Result;
            }
        case GraphicsApi::Vulkan:
            {
                Pipeline* Result = new VulkanPipeline(this);
                if (!Result->Initialize(Specification))
                {
                    delete Result;
                    return nullptr;
                }
                return Result;
            }
        case GraphicsApi::D3D12:
            {
                Pipeline* Result = new D3D12Pipeline(this);
                if (!Result->Initialize(Specification))
                {
                    delete Result;
                    return nullptr;
                }
                return Result;
            }
        default: return nullptr;
        }
    }

    VertexBuffer* Renderer::CreateVertexBuffer(const VertexBufferCreateInfo& CreateInfo)
    {
        VertexBuffer* Result = nullptr;
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None:     return nullptr;
        case GraphicsApi::OpenGL:   Result = new OpenGLVertexBuffer(this); break;
        case GraphicsApi::Vulkan:   Result = new VulkanVertexBuffer(this); break;
        case GraphicsApi::D3D12:    Result = new D3D12VertexBuffer(this); break;
        default:                    return nullptr;
        }

        if (!Result->Initialize(CreateInfo))
        {
            delete Result;
            return nullptr;
        }
        return Result;
    }

    VertexBuffer* Renderer::CreateVertexBuffer(const BufferView<Vertex>& Vertices)
    {
        VertexBufferCreateInfo CreateInfo;
        CreateInfo.Data = Vertices.Data();
        CreateInfo.Count = Vertices.Count();
        return CreateVertexBuffer(CreateInfo);
    }

    IndexBuffer* Renderer::CreateIndexBuffer(const IndexBufferCreateInfo& CreateInfo)
    {
        IndexBuffer* Result = nullptr;
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None:     return nullptr;
        case GraphicsApi::OpenGL:   Result = new OpenGLIndexBuffer(this); break;
        case GraphicsApi::Vulkan:   Result = new VulkanIndexBuffer(this); break;
        case GraphicsApi::D3D12:    Result = new D3D12IndexBuffer(this); break;
        default:                    return nullptr;
        }

        if (!Result->Initialize(CreateInfo))
        {
            delete Result;
            return nullptr;
        }
        return Result;
    }

    IndexBuffer* Renderer::CreateIndexBuffer(const BufferView<u32>& Indices)
    {
        IndexBufferCreateInfo CreateInfo;
        CreateInfo.Format = Format::R32_UINT;
        CreateInfo.Data = Indices.Data();
        CreateInfo.Size = Indices.Size();
        return CreateIndexBuffer(CreateInfo);
    }

    UniformBuffer* Renderer::CreateUniformBuffer(const size_t Size)
    {
        UniformBuffer* OutBuffer = nullptr;
        switch (m_GraphicsApi)
        {
        case GraphicsApi::None:     return nullptr;
        case GraphicsApi::OpenGL:   OutBuffer = new OpenGLUniformBuffer(this); break;
        case GraphicsApi::Vulkan:   OutBuffer = new VulkanUniformBuffer(this); break;
        case GraphicsApi::D3D12:    return nullptr;
        }

        if (!OutBuffer->Allocate(Size))
        {
            delete OutBuffer;
            return nullptr;
        }
        return OutBuffer;
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

