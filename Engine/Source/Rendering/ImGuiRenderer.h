#pragma once
#include <cstdint>

struct ImGuiContext;

namespace Nova { class Window; }

namespace Nova::Rendering
{
    class Device;
    class CommandBuffer;

    struct ImGuiRendererCreateInfo
    {
        Window* window = nullptr;
        Device* device = nullptr;
        uint32_t sampleCount = 0;
    };

    class ImGuiRenderer
    {
    public:
        ImGuiRenderer() = default;
        virtual ~ImGuiRenderer() = default;

        virtual bool Initialize(const ImGuiRendererCreateInfo& createInfo);
        virtual void Destroy() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Render(CommandBuffer& commandBuffer) = 0;
    protected:
        ImGuiContext* m_Context = nullptr;
    };
}