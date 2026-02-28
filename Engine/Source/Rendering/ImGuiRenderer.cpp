#include "ImGuiRenderer.h"
#include "RenderDevice.h"
#include "External/ImGuiExtension.h"

#include <imgui.h>

#ifdef NOVA_HAS_VULKAN
#include "Vulkan/ImGuiRenderer.h"
#endif

#ifdef NOVA_HAS_D3D12
#include "D3D12/ImGuiRenderer.h"
#endif

#ifdef NOVA_HAS_OPENGL
#include "OpenGL/ImGuiRenderer.h"
#endif

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

#define NOVA_RETURN_IMPL(className) \
{ \
    renderer = new (className)(); \
    if (!renderer->Initialize({window, device, sampleCount})) \
    { \
        delete renderer; \
        return nullptr; \
    } \
} break;

    Ref<ImGuiRenderer> CreateImGuiRenderer(Window* window, RenderDevice* device, const uint32_t sampleCount)
    {
        if (!window) return nullptr;
        if (!device) return nullptr;
        if (sampleCount % 2 != 0 && sampleCount > 16) return nullptr;
        ImGuiRenderer* renderer = nullptr;
        switch (device->GetDeviceType())
        {
        case RenderDeviceType::Null: return nullptr;
#ifdef NOVA_HAS_VULKAN
        case RenderDeviceType::Vulkan: NOVA_RETURN_IMPL(Vulkan::ImGuiRenderer)
#endif
#ifdef NOVA_HAS_D3D12
        case RenderDeviceType::D3D12: NOVA_RETURN_IMPL(D3D12::ImGuiRenderer)
#endif
#ifdef NOVA_HAS_OPENGL
        case RenderDeviceType::OpenGL: NOVA_RETURN_IMPL(OpenGL::ImGuiRenderer)
#endif
        }
        return Ref(renderer);
    }

#undef NOVA_RETURN_IMPL
}
