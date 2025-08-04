#pragma once
#include "Rendering/ImGuiRenderer.h"


namespace Nova::Vulkan
{
    class Device;

    class ImGuiRenderer : public Rendering::ImGuiRenderer
    {
    public:
        bool Initialize(const Rendering::ImGuiRendererCreateInfo& createInfo) override;
        void Destroy() override;
        void BeginFrame() override;
        void EndFrame() override;
        void Render(Rendering::CommandBuffer& commandBuffer) override;
    private:
        Device* m_Device = nullptr;
    };
}
