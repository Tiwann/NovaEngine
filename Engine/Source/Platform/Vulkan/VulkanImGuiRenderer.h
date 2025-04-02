#pragma once
#include "Editor/ImGuiRenderer.h"

namespace Hydro
{
    class VulkanImGuiRenderer : public ImGuiRenderer
    {
    public:
        bool Initialize(Application* Application) override;
        void Shutdown() override;
        void BeginFrame() override;
        void Render() override;
    };
}
