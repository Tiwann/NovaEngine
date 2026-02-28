#pragma once

namespace Nova
{
    enum class RenderDeviceType
    {
        Null,
#ifdef NOVA_HAS_VULKAN
        Vulkan,
#endif
#ifdef NOVA_HAS_D3D12
        D3D12,
#endif
#ifdef NOVA_HAS_OPENGL
        OpenGL
#endif
    };

#ifdef NOVA_HAS_VULKAN
    static constexpr bool EngineHasVulkan = true;
#else
    static constexpr bool EngineHasVulkan = false;
#endif

#ifdef NOVA_HAS_D3D12
    static constexpr bool EngineHasD3D12 = true;
#else
    static constexpr bool EngineHasD3D12 = false;
#endif

#ifdef NOVA_HAS_OPENGL
    static constexpr bool EngineHasOpenGL = true;
#else
    static constexpr bool EngineHasOpenGL = false;
#endif

    static constexpr bool EngineHasDeviceType(const RenderDeviceType type)
    {
        switch (type)
        {
        case RenderDeviceType::Null: return true;
#ifdef NOVA_HAS_VULKAN
        case RenderDeviceType::Vulkan: return EngineHasVulkan;
#endif
#ifdef NOVA_HAS_D3D12
        case RenderDeviceType::D3D12: return EngineHasD3D12;
#endif
#ifdef NOVA_HAS_OPENGL
        case RenderDeviceType::OpenGL: return EngineHasOpenGL;
#endif
        default: return false;
        }
    }

    static constexpr bool RenderDeviceIsOpenGL(RenderDeviceType type)
    {
        switch (type)
        {
        case RenderDeviceType::Null: return false;
#ifdef NOVA_HAS_VULKAN
        case RenderDeviceType::Vulkan: return false;
#endif
#ifdef NOVA_HAS_D3D12
        case RenderDeviceType::D3D12: return false;
#endif
#ifdef NOVA_HAS_OPENGL
        case RenderDeviceType::OpenGL: return true;
#endif
        default: return false;
        }
    }

    static constexpr bool RenderDeviceIsVulkan(RenderDeviceType type)
    {
        switch (type)
        {
        case RenderDeviceType::Null: return false;
#ifdef NOVA_HAS_VULKAN
        case RenderDeviceType::Vulkan: return true;
#endif
#ifdef NOVA_HAS_D3D12
        case RenderDeviceType::D3D12: return false;
#endif
#ifdef NOVA_HAS_OPENGL
        case RenderDeviceType::OpenGL: return false;
#endif
        default: return false;
        }
    }

    static constexpr bool RenderDeviceIsD3D12(RenderDeviceType type)
    {
        switch (type)
        {
        case RenderDeviceType::Null: return false;
#ifdef NOVA_HAS_VULKAN
        case RenderDeviceType::Vulkan: return false;
#endif
#ifdef NOVA_HAS_D3D12
        case RenderDeviceType::D3D12: return true;
#endif
#ifdef NOVA_HAS_OPENGL
        case RenderDeviceType::OpenGL: return false;
#endif
        default: return false;
        }
    }
}