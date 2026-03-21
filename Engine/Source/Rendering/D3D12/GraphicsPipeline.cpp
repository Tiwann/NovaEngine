#include "GraphicsPipeline.h"
#include "RenderDevice.h"
#include <directx/d3dx12.h>

#include "Conversions.h"

namespace Nova::D3D12
{
    bool GraphicsPipeline::Initialize(const GraphicsPipelineCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (!createInfo.shader) return false;


        const auto& inputAttributes = createInfo.vertexInputState.vertexInputAttributes;
        const auto& inputBindings = createInfo.vertexInputState.vertexInputBindings;
        const auto& GetBinding = [](const uint32_t bindingIndex, const Array<VertexInputBindingDesc>& bindings)
        {
            return *bindings.Single( [&bindingIndex](const VertexInputBindingDesc& bindingDesc)
                {
                    return bindingDesc.binding == bindingIndex;
                });
        };

        Array<D3D12_INPUT_ELEMENT_DESC> inputLayouts;
        for (const VertexInputAttributeDesc& attribute : inputAttributes)
        {
            const uint32_t binding = attribute.binding;
            const auto& inputBinding = GetBinding(binding, inputBindings);

            D3D12_INPUT_ELEMENT_DESC desc = {};
            desc.SemanticIndex = attribute.location;
            desc.Format = Convert<DXGI_FORMAT>(attribute.format);
            desc.AlignedByteOffset = attribute.offset;
            desc.InputSlot = binding;
            desc.InputSlotClass = Convert<D3D12_INPUT_CLASSIFICATION>(inputBinding.inputRate);
            desc.InstanceDataStepRate = inputBinding.stride;

            inputLayouts.Add(desc);
        }

        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
        desc.InputLayout.NumElements = inputLayouts.Count();
        desc.InputLayout.pInputElementDescs = inputLayouts.Data();

        D3D12_RASTERIZER_DESC raster = {};
        raster.FillMode = Convert<D3D12_FILL_MODE>(createInfo.rasterizationState.polygonMode);
        raster.CullMode = Convert<D3D12_CULL_MODE>(createInfo.rasterizationState.cullMode);
        raster.FrontCounterClockwise = (createInfo.rasterizationState.frontFace == FrontFace::CounterClockwise);
        raster.DepthBias = (INT)createInfo.rasterizationState.depthBiasConstantFactor;
        raster.DepthBiasClamp = createInfo.rasterizationState.depthBiasClamp;
        raster.SlopeScaledDepthBias = createInfo.rasterizationState.depthBiasSlopeFactor;
        raster.DepthClipEnable = !createInfo.rasterizationState.depthClampEnable;
        raster.MultisampleEnable = createInfo.multisampleState.sampleCount > 1;
        raster.AntialiasedLineEnable = FALSE;
        raster.ForcedSampleCount = 0;
        raster.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        desc.RasterizerState = raster;

        D3D12_BLEND_DESC blend = {};
        blend.AlphaToCoverageEnable = createInfo.multisampleState.alphaToCoverageEnable;
        blend.IndependentBlendEnable = TRUE;

        for (UINT i = 0; i < createInfo.colorBlendStates.Count(); i++)
        {
            const auto& src = createInfo.colorBlendStates[i];
            auto& dst = blend.RenderTarget[i];

            dst.BlendEnable = src.colorBlendEnable;
            dst.LogicOpEnable = FALSE;

            dst.SrcBlend = Convert<D3D12_BLEND>(src.blendFunction.colorSource);
            dst.DestBlend = Convert<D3D12_BLEND>(src.blendFunction.colorDest);
            dst.BlendOp = Convert<D3D12_BLEND_OP>(src.blendFunction.colorOp);

            dst.SrcBlendAlpha = Convert<D3D12_BLEND>(src.blendFunction.alphaSource);
            dst.DestBlendAlpha = Convert<D3D12_BLEND>(src.blendFunction.alphaDest);
            dst.BlendOpAlpha = Convert<D3D12_BLEND_OP>(src.blendFunction.alphaOp);

            dst.RenderTargetWriteMask = Convert<UINT8>(src.colorWriteMask);
        }

        desc.BlendState = blend;

        D3D12_DEPTH_STENCIL_DESC depth = {};
        depth.DepthEnable = createInfo.depthStencilState.depthTestEnable;
        depth.DepthWriteMask = createInfo.depthStencilState.depthWriteEnable
            ? D3D12_DEPTH_WRITE_MASK_ALL
            : D3D12_DEPTH_WRITE_MASK_ZERO;

        depth.DepthFunc = Convert<D3D12_COMPARISON_FUNC>(
            createInfo.depthStencilState.depthCompareOp
        );

        depth.StencilEnable = createInfo.depthStencilState.stencilTestEnable;

        // Default stencil (you can expand later)
        depth.FrontFace = depth.BackFace = {
            D3D12_STENCIL_OP_KEEP,
            D3D12_STENCIL_OP_KEEP,
            D3D12_STENCIL_OP_KEEP,
            D3D12_COMPARISON_FUNC_ALWAYS
        };

        desc.DepthStencilState = depth;

        desc.NumRenderTargets = (UINT)createInfo.colorAttachmentFormats.Count();

        for (UINT i = 0; i < desc.NumRenderTargets; i++)
        {
            desc.RTVFormats[i] = Convert<DXGI_FORMAT>(
                createInfo.colorAttachmentFormats[i]
            );
        }

        desc.DSVFormat = Convert<DXGI_FORMAT>(createInfo.depthAttachmentFormat);

        desc.SampleDesc.Count = createInfo.multisampleState.sampleCount;
        desc.SampleDesc.Quality = 0;
        desc.SampleMask = UINT_MAX;
        desc.NodeMask = 0;
        desc.CachedPSO = {};
        desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        ID3D12Device13* deviceHandle = device->GetHandle();
        ID3D12PipelineState* handle = nullptr;
        if (DX_FAILED(deviceHandle->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&handle))))
            return false;
        if (m_Handle) m_Handle->Release();
        m_Handle = handle;
        return true;
    }

    void GraphicsPipeline::Destroy()
    {
    }
}
