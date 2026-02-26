#pragma once
#include "Runtime/Object.h"
#include "Runtime/Ref.h"
#include <cstdint>


struct ImGuiContext;

namespace Nova { class Window; }

namespace Nova
{
    class RenderDevice;
    class CommandBuffer;

    struct ImGuiRendererCreateInfo
    {
        Window* window = nullptr;
        RenderDevice* device = nullptr;
        uint32_t sampleCount = 0;
    };

    class ImGuiRenderer : public Object
    {
    public:
        ImGuiRenderer() : Object("ImGui Renderer"){}
        ~ImGuiRenderer() override = default;

        virtual bool Initialize(const ImGuiRendererCreateInfo& createInfo);
        virtual void Destroy() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Render(Nova::CommandBuffer& commandBuffer) = 0;
    protected:
        ImGuiContext* m_Context = nullptr;
    };

    Ref<ImGuiRenderer> CreateImGuiRenderer(Window* window, RenderDevice* device, uint32_t sampleCount);
}