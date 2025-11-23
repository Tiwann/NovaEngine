#pragma once
#include "Rendering/ImGuiRenderer.h"


namespace Nova::Vulkan
{
    class Device;

    class ImGuiRenderer final : public Nova::ImGuiRenderer
    {
    public:
        bool Initialize(const ImGuiRendererCreateInfo& createInfo) override;
        void Destroy() override;
        void BeginFrame() override;
        void EndFrame() override;
        void Render(CommandBuffer& commandBuffer) override;
    private:
        Device* m_Device = nullptr;
    };
}
