#pragma once
#include "EditorWindow.h"
#include "External/ImGuiExtension.h"
#include <cstdint>

namespace Nova
{
    namespace Rendering { class RenderTarget; class Sampler; class Device; }

    class ViewportWindow final : public EditorWindow
    {
    public:
        explicit ViewportWindow(Ref<Rendering::Device> device) : EditorWindow("Viewport"), m_Device(device) {}
        ~ViewportWindow() override = default;

        void OnInit() override;
        void OnGui() override;
        void OnDestroy() override;

        void SetRenderTarget(Ref<Rendering::RenderTarget> renderTarget);
        void UpdateResources();
    private:
        Ref<Rendering::Device> m_Device = nullptr;
        Ref<Rendering::Sampler> m_Sampler = nullptr;
        Ref<Rendering::RenderTarget> m_RenderTarget = nullptr;
        uint64_t m_TextureIds[3] = { 0, 0, 0 };
    };
}
