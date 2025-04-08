#pragma once
#include "Runtime/LogCategory.h"
#include "Runtime/Log.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(ImGui, "IMGUI");
#define NOVA_IMGUI_ERROR(str, ...) NOVA_LOG(ImGui, Verbosity::Error, str, __VA_ARGS__)


struct ImGuiContext;

namespace Nova
{
    class Application;
    
    class ImGuiRenderer
    {
    public:
        ImGuiRenderer() = default;
        virtual ~ImGuiRenderer() = default;

        static ImGuiRenderer* Create();
        virtual bool Initialize(Application* Application);
        virtual void Shutdown() = 0;
        virtual void BeginFrame() = 0;
        virtual void EndFrame();
        virtual void Render() = 0;
    private:
        void CustomStyle();
    protected:
        Application* m_Application = nullptr;
        ImGuiContext* m_Context = nullptr;
    };
}
