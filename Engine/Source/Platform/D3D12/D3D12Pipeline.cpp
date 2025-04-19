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
        Array<D3D12_INPUT_ELEMENT_DESC> InputElementDescriptions;
        for (size_t i = 0; i < Specification.VertexLayout.Count(); i++)
        {
            const VertexAttribute& Attribute = Specification.VertexLayout[i];
            D3D12_INPUT_ELEMENT_DESC InputElementDescription {};
            InputElementDescription.InputSlot = i;
            InputElementDescription.SemanticName = *Attribute.Name;
            InputElementDescription.AlignedByteOffset = Specification.VertexLayout.GetOffset(Attribute);
            InputElementDescription.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            InputElementDescription.InstanceDataStepRate = 0;
            //InputElementDescription.InputSlot = ...;

            InputElementDescriptions.Add(InputElementDescription);
        }

        PipelineStateDescription.InputLayout.NumElements = InputElementDescriptions.Count();
        PipelineStateDescription.InputLayout.pInputElementDescs = InputElementDescriptions.Data();


        ID3D12Device9* Device = ((D3D12Renderer*)m_Renderer)->GetDevice();
        if (DX_FAILED(Device->CreateGraphicsPipelineState(&PipelineStateDescription, IID_PPV_ARGS(&m_Handle))))
        {
            NOVA_DIRECTX_ERROR("Failed to create graphics pipeline");
            return;
        }
    }

    D3D12Pipeline::~D3D12Pipeline()
    {
        m_Handle->Release();
    }

    ID3D12PipelineState* D3D12Pipeline::GetHandle() const
    {
        return m_Handle;
    }
}
