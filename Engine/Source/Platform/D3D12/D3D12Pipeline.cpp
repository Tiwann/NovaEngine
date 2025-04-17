#include "D3D12Pipeline.h"
#include "D3D12Renderer.h"
#include "Runtime/Log.h"
#include <directx/d3d12.h>


namespace Nova
{
    D3D12Pipeline::D3D12Pipeline(Renderer* Renderer, const PipelineSpecification& Specification): Pipeline(Renderer, Specification)
    {
        // TODO: Create PSO
        D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDescription = {};

        ID3D12Device9* Device = ((D3D12Renderer*)m_Renderer)->GetDevice();
        if (DX_FAILED(Device->CreateGraphicsPipelineState(&PipelineStateDescription, IID_PPV_ARGS(&m_Handle))))
        {
            NOVA_DIRECTX_ERROR("Failed to create graphics pipeline");
            return;
        }
    }

    ID3D12PipelineState* D3D12Pipeline::GetHandle() const
    {
        return m_Handle;
    }
}
