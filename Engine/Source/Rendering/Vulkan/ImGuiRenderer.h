#pragma once
#include "Rendering/ImGuiRenderer.h"


namespace Nova::Vulkan
{
    class RenderDevice;

    class ImGuiRenderer final : public Nova::ImGuiRenderer
    {
    public:
        bool Initialize(const ImGuiRendererCreateInfo& createInfo) override;
        void Destroy() override;
        void BeginFrame() override;
        void EndFrame() override;
        void Render(CommandBuffer& commandBuffer) override;
    private:
        RenderDevice* m_Device = nullptr;
    };
}
