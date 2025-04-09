#include "VertexArray.h"
#include "Platform/PlatformVertexArray.h"

namespace Nova
{
    VertexArray* VertexArray::Create(GraphicsApi const& GraphicsApi)
    {
        switch (GraphicsApi)
        {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return new OpenGLVertexArray();
        case GraphicsApi::Vulkan: return new VulkanVertexArray();
        case GraphicsApi::D3D12: return new D3D12VertexArray();
        default: return nullptr;
        }

    }
}
