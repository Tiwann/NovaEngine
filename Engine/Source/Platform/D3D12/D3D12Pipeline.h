#pragma once
#include "Rendering/Pipeline.h"

struct ID3D12PipelineState;

namespace Nova
{
    class D3D12Pipeline : public Pipeline
    {
    public:
        explicit D3D12Pipeline(Renderer* Renderer);

        void Destroy() override;
        bool Initialize(const PipelineCreateInfo& Specification) override;

        ID3D12PipelineState* GetHandle() const;

    private:
        ID3D12PipelineState* m_Handle = nullptr;
    };
}
