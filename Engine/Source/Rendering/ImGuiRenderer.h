#pragma once
#include "Runtime/Object.h"
#include <cstdint>

#include "Runtime/Ref.h"


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

    class ImGuiRenderer : public Object
    {
    public:
        ImGuiRenderer() : Object("ImGui Renderer"){}
        ~ImGuiRenderer() override = default;

        virtual bool Initialize(const ImGuiRendererCreateInfo& createInfo);
        virtual void Destroy() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Render(CommandBuffer& commandBuffer) = 0;
    protected:
        ImGuiContext* m_Context = nullptr;
    };

    Ref<ImGuiRenderer> CreateImGuiRenderer(Window* window, Device* device, uint32_t sampleCount);
}