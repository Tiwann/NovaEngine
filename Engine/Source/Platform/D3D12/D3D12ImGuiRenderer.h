#pragma once
#include "Editor/ImGuiRenderer.h"

namespace Nova
{
    class D3D12ImGuiRenderer : public ImGuiRenderer
    {
    public:
        bool Initialize(Application* Application) override;
        void Shutdown() override;
        void BeginFrame() override;
        void Render() override;
    };
}
