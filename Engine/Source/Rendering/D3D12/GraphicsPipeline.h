#pragma once
#include "Rendering/GraphicsPipeline.h"

struct ID3D12PipelineState;

namespace Nova::D3D12
{
    class GraphicsPipeline : public Nova::GraphicsPipeline
    {
    public:
        bool Initialize(const GraphicsPipelineCreateInfo& createInfo) override;
        void Destroy() override;

        ID3D12PipelineState* GetHandle() const { return m_Handle; }
    private:
        ID3D12PipelineState* m_Handle = nullptr;
    };
}