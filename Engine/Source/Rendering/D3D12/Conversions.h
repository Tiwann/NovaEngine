#pragma once
#include "Runtime/Format.h"
#include "Rendering/PresentMode.h"
#include "Rendering/ResourceState.h"
#include <dxgi1_6.h>
#include <directx/d3dx12.h>


namespace Nova::D3D12
{
    template<typename OutType, typename InType>
    OutType Convert(const InType& value);

    template <>
    inline DXGI_FORMAT Convert<DXGI_FORMAT, Format>(const Format& value)
    {
        switch (value)
        {
        case Format::None: return DXGI_FORMAT_UNKNOWN;

        case Format::R8_UNORM: return DXGI_FORMAT_R8_UNORM;
        case Format::R8_SNORM: return DXGI_FORMAT_R8_SNORM;
        case Format::R16_USHORT: return DXGI_FORMAT_R16_UNORM;
        case Format::R16_SHORT: return DXGI_FORMAT_R16_SNORM;
        case Format::R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
        case Format::R32_UINT: return DXGI_FORMAT_R32_UINT;
        case Format::R32_SINT: return DXGI_FORMAT_R32_SINT;

        case Format::R8G8_UNORM: return DXGI_FORMAT_R8G8_UNORM;
        case Format::R8G8_SNORM: return DXGI_FORMAT_R8G8_SNORM;
        case Format::R16G16_USHORT: return DXGI_FORMAT_R16G16_UNORM;
        case Format::R16G16_SHORT: return DXGI_FORMAT_R16G16_SNORM;
        case Format::R32G32_UINT: return DXGI_FORMAT_R32G32_UINT;
        case Format::R32G32_SINT: return DXGI_FORMAT_R32G32_SINT;
        case Format::R32G32_FLOAT: return DXGI_FORMAT_R32G32_FLOAT;

        case Format::R8G8B8_UNORM: return DXGI_FORMAT_UNKNOWN;
        case Format::R8G8B8_SNORM: return DXGI_FORMAT_UNKNOWN;
        case Format::R16G16B16_USHORT: return DXGI_FORMAT_UNKNOWN;
        case Format::R16G16B16_SHORT: return DXGI_FORMAT_UNKNOWN;
        case Format::R32G32B32_UINT: return DXGI_FORMAT_R32G32B32_UINT;
        case Format::R32G32B32_SINT: return DXGI_FORMAT_R32G32B32_SINT;
        case Format::R32G32B32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT;

        case Format::R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8_SNORM: return DXGI_FORMAT_R8G8B8A8_SNORM;
        case Format::R8G8B8A8_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case Format::R16G16B16A16_USHORT: return DXGI_FORMAT_R16G16B16A16_UNORM;
        case Format::R16G16B16A16_SHORT: return DXGI_FORMAT_R16G16B16A16_SNORM;
        case Format::R32G32B32A32_UINT: return DXGI_FORMAT_R32G32B32A32_UINT;
        case Format::R32G32B32A32_SINT: return DXGI_FORMAT_R32G32B32A32_SINT;
        case Format::R32G32B32A32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case Format::R64G64B64A64_FLOAT: return DXGI_FORMAT_UNKNOWN;

        case Format::D32_FLOAT: return DXGI_FORMAT_D32_FLOAT;
        case Format::D32_FLOAT_S8_UINT: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case Format::D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;

        case Format::ASTC_4X4_UNORM:
        case Format::ASTC_4X4_FLOAT:
        case Format::ASTC_4X4_SRGB:
        case Format::ASTC_5X4_UNORM:
        case Format::ASTC_5X4_FLOAT:
        case Format::ASTC_5X4_SRGB:
        case Format::ASTC_5X5_UNORM:
        case Format::ASTC_5X5_FLOAT:
        case Format::ASTC_5X5_SRGB:
        case Format::ASTC_6X5_UNORM:
        case Format::ASTC_6X5_FLOAT:
        case Format::ASTC_6X5_SRGB:
        case Format::ASTC_6X6_UNORM:
        case Format::ASTC_6X6_FLOAT:
        case Format::ASTC_6X6_SRGB:
        case Format::ASTC_8X5_UNORM:
        case Format::ASTC_8X5_FLOAT:
        case Format::ASTC_8X5_SRGB:
        case Format::ASTC_8X6_UNORM:
        case Format::ASTC_8X6_FLOAT:
        case Format::ASTC_8X6_SRGB:
        case Format::ASTC_8X8_UNORM:
        case Format::ASTC_8X8_FLOAT:
        case Format::ASTC_8X8_SRGB:
        case Format::ASTC_10X5_UNORM:
        case Format::ASTC_10X5_FLOAT:
        case Format::ASTC_10X5_SRGB:
        case Format::ASTC_10X6_UNORM:
        case Format::ASTC_10X6_FLOAT:
        case Format::ASTC_10X6_SRGB:
        case Format::ASTC_10X8_UNORM:
        case Format::ASTC_10X8_FLOAT:
        case Format::ASTC_10X8_SRGB:
        case Format::ASTC_10X10_UNORM:
        case Format::ASTC_10X10_FLOAT:
        case Format::ASTC_10X10_SRGB:
        case Format::ASTC_12X10_UNORM:
        case Format::ASTC_12X10_FLOAT:
        case Format::ASTC_12X10_SRGB:
        case Format::ASTC_12X12_UNORM:
        case Format::ASTC_12X12_FLOAT:
        case Format::ASTC_12X12_SRGB: return DXGI_FORMAT_UNKNOWN;

        case Format::BC1_UNORM: return DXGI_FORMAT_BC1_UNORM;
        case Format::BC1_SRGB: return DXGI_FORMAT_BC1_UNORM_SRGB;
        case Format::BC2_UNORM: return DXGI_FORMAT_BC2_UNORM;
        case Format::BC2_SRGB: return DXGI_FORMAT_BC2_UNORM_SRGB;
        case Format::BC3_UNORM: return DXGI_FORMAT_BC3_UNORM;
        case Format::BC3_SRGB: return DXGI_FORMAT_BC3_UNORM_SRGB;
        case Format::BC4_UNORM: return DXGI_FORMAT_BC4_UNORM;
        case Format::BC4_SNORM: return DXGI_FORMAT_BC4_SNORM;
        case Format::BC5_UNORM: return DXGI_FORMAT_BC5_UNORM;
        case Format::BC5_SNORM: return DXGI_FORMAT_BC5_SNORM;
        case Format::BC7_UNORM: return DXGI_FORMAT_BC7_UNORM;
        case Format::BC7_SRGB: return DXGI_FORMAT_BC7_UNORM_SRGB;
        }

        return DXGI_FORMAT_UNKNOWN;
    }

    template <>
    inline DXGI_SWAP_EFFECT Convert<DXGI_SWAP_EFFECT, PresentMode>(const PresentMode& value)
    {
        switch (value)
        {
        case PresentMode::Unknown: throw;
        case PresentMode::Immediate: return DXGI_SWAP_EFFECT_DISCARD;
        case PresentMode::Mailbox: return DXGI_SWAP_EFFECT_FLIP_DISCARD;
        case PresentMode::Fifo: return DXGI_SWAP_EFFECT_FLIP_DISCARD;
        default: throw;
        }
    }

    template<>
    inline D3D12_RESOURCE_STATES Convert<D3D12_RESOURCE_STATES, ResourceState>(const ResourceState& resourceState)
    {
        switch (resourceState)
        {
        case ResourceState::Undefined: return D3D12_RESOURCE_STATE_COMMON;
        case ResourceState::General: return D3D12_RESOURCE_STATE_COMMON;
        case ResourceState::ShaderRead: return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
        case ResourceState::ShaderWrite: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        case ResourceState::ColorAttachment: return D3D12_RESOURCE_STATE_RENDER_TARGET;
        case ResourceState::DepthStencilAttachment: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
        case ResourceState::TransferSource: return D3D12_RESOURCE_STATE_COPY_SOURCE;
        case ResourceState::TransferDest: return D3D12_RESOURCE_STATE_COPY_DEST;
        case ResourceState::Present: return D3D12_RESOURCE_STATE_PRESENT;
        default: return D3D12_RESOURCE_STATE_COMMON;
        }
    }
    template<>
    inline ResourceState Convert<ResourceState, D3D12_RESOURCE_STATES>(const D3D12_RESOURCE_STATES& resourceState)
    {
        return ResourceState::Undefined;
    }
}
