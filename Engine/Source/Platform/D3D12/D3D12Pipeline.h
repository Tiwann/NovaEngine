#pragma once
#include "Rendering/Pipeline.h"

struct ID3D12PipelineState;
namespace Nova
{
    class D3D12Pipeline : public Pipeline
    {
    public:
        D3D12Pipeline(Renderer* Renderer, const PipelineSpecification& Specification);
        ~D3D12Pipeline() override;
        ID3D12PipelineState* GetHandle() const;
    private:
        ID3D12PipelineState* m_Handle;
    };
}
