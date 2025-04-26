#pragma once
#include "Rendering/RendererTypeConvertor.h"
#include "D3D12FrontFace.h"

#include <dxgi.h>
#include <directx/d3dx12.h>

namespace Nova
{

    struct D3D12RendererTypeConvertor : RendererTypeConvertor<D3D12_CULL_MODE, D3D12_BLEND, D3D12_BLEND_OP,
    D3D12_FILTER_TYPE, D3D12_FRONT_FACE, D3D12_FILL_MODE, D3D12_PRIMITIVE_TOPOLOGY_TYPE, D3D12_TEXTURE_ADDRESS_MODE,
    D3D12_SHADER_VISIBILITY, D3D12_COMPARISON_FUNC, DXGI_FORMAT, DXGI_SWAP_EFFECT>
    {
        D3D12_CULL_MODE ConvertCullMode(const CullMode Mode) const override;
        D3D12_BLEND ConvertBlendFactor(const BlendFactor Factor) const override;
        D3D12_BLEND_OP ConvertBlendOperation(const BlendOperation Operation) const override;
        D3D12_FILTER_TYPE ConvertFilter(const Filter Filter) const override;
        D3D12_FRONT_FACE ConvertFrontFace(const FrontFace Face) const override;
        D3D12_FILL_MODE ConvertPolygonMode(const PolygonMode Mode) const override;
        D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimitiveTopology(const PrimitiveTopology Topology) const override;
        D3D12_TEXTURE_ADDRESS_MODE ConvertSamplerAddressMode(const SamplerAddressMode Address) const override;
        D3D12_SHADER_VISIBILITY ConvertShaderStage(const ShaderStage Stage) const override;
        D3D12_COMPARISON_FUNC ConvertCompareOperation(const CompareOperation Operation) const override;
        DXGI_FORMAT ConvertFormat(const Format Format) const override;
        DXGI_SWAP_EFFECT ConvertPresentMode(const PresentMode PresentMode) const override;
    };
}
