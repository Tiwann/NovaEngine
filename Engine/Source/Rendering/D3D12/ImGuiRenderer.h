#pragma once
#include "Rendering/ImGuiRenderer.h"

struct ID3D12DescriptorHeap;

namespace Nova::D3D12
{
    class ImGuiRenderer final : public Nova::ImGuiRenderer
    {
    public:
        bool Initialize(const ImGuiRendererCreateInfo& createInfo) override;
        void Destroy() override;
        void BeginFrame() override;
        void EndFrame() override;
        void Render(Nova::CommandBuffer& commandBuffer) override;
    private:
        ID3D12DescriptorHeap* m_DescriptorHeap = nullptr;
    };
}
