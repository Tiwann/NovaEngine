#pragma once
#include "EditorWindow.h"
#include "External/ImGuiExtension.h"

namespace Nova
{
    class RenderTarget;
    class Sampler;
    class RenderDevice;

    class ViewportWindow final : public EditorWindow
    {
    public:
        explicit ViewportWindow(Ref<RenderDevice> device) : EditorWindow("Viewport"), m_Device(device) {}
        ~ViewportWindow() override = default;

        void OnInit() override;
        void OnGui() override;
        void OnDestroy() override;

        void SetRenderTarget(Ref<RenderTarget> renderTarget);
    private:
        Ref<RenderDevice> m_Device = nullptr;
        Ref<Sampler> m_Sampler = nullptr;
        Ref<RenderTarget> m_RenderTarget = nullptr;
    };
}
