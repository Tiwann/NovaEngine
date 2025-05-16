#include "D3D12Pipeline.h"
#include "D3D12Renderer.h"
#include "Runtime/Log.h"
#include "D3D12RendererTypeConvertor.h"
#include "D3D12Shader.h"
#include "Rendering/Shader.h"
#include "Rendering/ShaderModule.h"

#include <directx/d3d12.h>
#include <slang/slang.h>




namespace Nova
{
    D3D12Pipeline::D3D12Pipeline(Renderer* Renderer): Pipeline(Renderer)
    {

    }


    void D3D12Pipeline::Destroy()
    {
        m_Handle->Release();
    }

    bool D3D12Pipeline::Initialize(const PipelineCreateInfo& Specification)
    {
        Pipeline::Initialize(Specification);

        const D3D12RendererTypeConvertor& Convertor = m_Renderer->As<D3D12Renderer>()->Convertor;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDescription = {};

        Array<D3D12_INPUT_ELEMENT_DESC> InputElementDescriptions;
        for (size_t i = 0; i < Specification.VertexLayout.Count(); i++)
        {
            const VertexAttribute& Attribute = Specification.VertexLayout[i];
            D3D12_INPUT_ELEMENT_DESC InputElementDescription {};
            InputElementDescription.Format = Convertor.ConvertFormat(Attribute.Format);
            InputElementDescription.InputSlot = i;
            InputElementDescription.SemanticIndex = i;
            InputElementDescription.SemanticName = *Attribute.Name;
            InputElementDescription.AlignedByteOffset = Specification.VertexLayout.GetOffset(Attribute);
            InputElementDescription.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            InputElementDescription.InstanceDataStepRate = 0;
            //InputElementDescription.InputSlot = ...;
            InputElementDescriptions.Add(InputElementDescription);
        }

        PipelineStateDescription.InputLayout.NumElements = InputElementDescriptions.Count();
        PipelineStateDescription.InputLayout.pInputElementDescs = InputElementDescriptions.Data();
        PipelineStateDescription.BlendState.IndependentBlendEnable = Specification.BlendEnable;
        PipelineStateDescription.BlendState.AlphaToCoverageEnable = Specification.AlphaToCoverageEnable;

        PipelineStateDescription.RasterizerState.CullMode = Convertor.ConvertCullMode(Specification.CullMode);
        PipelineStateDescription.RasterizerState.FillMode = Convertor.ConvertPolygonMode(Specification.PolygonMode);
        PipelineStateDescription.RasterizerState.DepthClipEnable = Specification.DepthClampEnable;
        PipelineStateDescription.RasterizerState.DepthBiasClamp = Specification.DepthBiasClamp;
        PipelineStateDescription.RasterizerState.FrontCounterClockwise = Specification.FrontFace == FrontFace::CounterClockwise;

        PipelineStateDescription.SampleMask = 0xFFFFFFFF;
        PipelineStateDescription.SampleDesc.Count = Specification.RasterizationSamples;
        // PipelineStateDescription.SampleDesc.Quality = ...;

        PipelineStateDescription.DepthStencilState.DepthEnable = Specification.DepthTestEnable;
        PipelineStateDescription.DepthStencilState.StencilEnable = Specification.StencilTestEnable;
        PipelineStateDescription.DepthStencilState.DepthFunc = Convertor.ConvertCompareOperation(Specification.DepthCompareOperation);

        PipelineStateDescription.PrimitiveTopologyType = Convertor.ConvertPrimitiveTopology(Specification.PrimitiveTopology);

        if (!Specification.ShaderProgram)
            return false;

        const D3D12Shader* ShaderProgram = Specification.ShaderProgram->As<D3D12Shader>();
        if (const D3D12ShaderModule* VertexModule = ShaderProgram->GetVertexShaderModule())
        {
            slang::IBlob* CompiledCode = VertexModule->CompiledCode;
            PipelineStateDescription.VS.pShaderBytecode = CompiledCode->getBufferPointer();
            PipelineStateDescription.VS.BytecodeLength = CompiledCode->getBufferSize();
        }

        if (const D3D12ShaderModule* FragmentModule = ShaderProgram->GetFragmentShaderModule())
        {
            slang::IBlob* CompiledCode = FragmentModule->CompiledCode;
            PipelineStateDescription.PS.pShaderBytecode = CompiledCode->getBufferPointer();
            PipelineStateDescription.PS.BytecodeLength = CompiledCode->getBufferSize();
        }

        if (const D3D12ShaderModule* GeometryModule = ShaderProgram->GetShaderModule(ShaderStage::Geometry))
        {
            slang::IBlob* CompiledCode = GeometryModule->CompiledCode;
            PipelineStateDescription.GS.pShaderBytecode = CompiledCode->getBufferPointer();
            PipelineStateDescription.GS.BytecodeLength = CompiledCode->getBufferSize();
        }

        ID3D12Device9* Device = m_Renderer->As<D3D12Renderer>()->GetDevice();


        if (DX_FAILED(Device->CreateGraphicsPipelineState(&PipelineStateDescription, IID_PPV_ARGS(&m_Handle))))
        {
            NOVA_DIRECTX_ERROR("Failed to create graphics pipeline");
            return false;
        }

        return true;
    }

    ID3D12PipelineState* D3D12Pipeline::GetHandle() const
    {
        return m_Handle;
    }
}
