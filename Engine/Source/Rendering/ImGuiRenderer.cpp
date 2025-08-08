#include "ImGuiRenderer.h"
#include <imgui.h>

namespace Nova::Rendering
{
    bool ImGuiRenderer::Initialize(const ImGuiRendererCreateInfo& createInfo)
    {
        if(!IMGUI_CHECKVERSION())
            return false;

        m_Context = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        // TODO: Need to change branch to docking!!
        // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

        // TODO: Add imguizmo
        //ImGuizmo::SetImGuiContext(m_Context);
        return true;
    }
}