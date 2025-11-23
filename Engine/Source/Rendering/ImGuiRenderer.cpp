#include "ImGuiRenderer.h"
#include "Vulkan/ImGuiRenderer.h"
#include "Device.h"
#include "External/ImGuiExtension.h"

#include <imgui.h>

namespace Nova
{
    bool ImGuiRenderer::Initialize(const ImGuiRendererCreateInfo& createInfo)
    {
        if(!IMGUI_CHECKVERSION())
            return false;

        m_Context = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // TODO: Add fonts
        /*for(size_t i = 0; i < NumFonts; i++)
        {
            const Path FontPath = Application->GetEngineAssetsDirectory() / FontsPaths[i];
            Fonts[i] = io.Fonts->AddFontFromFileTTF(FontPath.string().c_str(), 16.0f);
        }
        io.FontDefault = Fonts[JetBrainsMono_SemiBold];*/

        if(!m_Context)
            return false;

        ImGui::SetCurrentContext(m_Context);
        ImGui::NovaStyle();

        // TODO: Add imguizmo
        //ImGuizmo::SetImGuiContext(m_Context);
        return true;
    }

    Ref<ImGuiRenderer> CreateImGuiRenderer(Window* window, Device* device, const uint32_t sampleCount)
    {
        if (!window) return nullptr;
        if (!device) return nullptr;
        if (sampleCount % 2 != 0 && sampleCount > 16) return nullptr;
        ImGuiRenderer* renderer = nullptr;
        switch (device->GetDeviceType())
        {
        case DeviceType::Unknown:
            return nullptr;
        case DeviceType::Vulkan:
            {
                renderer = new Vulkan::ImGuiRenderer();
                if (!renderer->Initialize({window, device, sampleCount}))
                {
                    delete renderer;
                    return nullptr;
                }
            }
        }
        return Ref(renderer);
    }
}
