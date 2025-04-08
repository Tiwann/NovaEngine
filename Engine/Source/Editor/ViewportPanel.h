#pragma once
#include "EditorWindow.h"
#include "Runtime/LogCategory.h"
#include "Math/Vector2.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(ViewportWindow, "VIEWPORT")

namespace Nova
{
    class FrameBuffer;

    class ViewportPanel : public EditorWindow
    {
    public:
        ViewportPanel() : EditorWindow("Viewport")
        {
            m_Opened = true;
        }

        void OnInit() override;
        void OnUpdate(f32 Delta) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;

        bool IsAvailable() const;

        Vector2 GetSize() const;
        Vector2 GetPosition() const { return m_Position; }
        FrameBuffer* GetFrameBuffer() const;
        ImDrawList* GetDrawList() const;

        static bool IsMouseHoveringWindow();
    private:
        bool m_MouseInside;
        FrameBuffer* m_FrameBuffer = nullptr;
        Vector2 m_Size;
        Vector2 m_Position;
        ImDrawList* m_DrawList = nullptr;
        bool m_Available = false;
    };
}
