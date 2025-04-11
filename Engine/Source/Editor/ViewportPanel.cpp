#include "ViewportPanel.h"
#include "EditorGUI.h"
#include "StyleVar.h"
#include "Runtime/Application.h"
#include "Rendering/FrameBuffer.h"
#include "Rendering/Texture2D.h"
#include "Math/MathConversions.h"


namespace
{
    inline const Nova::Vector2 WindowStartSize(960, 540);
}

namespace Nova
{
    void ViewportPanel::OnInit()
    {
        EditorWindow::OnInit();

        const Renderer* Renderer = g_Application->GetRenderer();
        m_FrameBuffer = FrameBuffer::Create(Renderer->GetGraphicsApi());
        m_FrameBuffer->CreateAttachment(FrameBufferAttachment::Color);
        m_FrameBuffer->CreateAttachment(FrameBufferAttachment::Depth);
        
        m_FrameBuffer->Resize(WindowStartSize);
        if(!m_FrameBuffer->Validate())
        {
            NOVA_LOG(ViewportWindow, Verbosity::Error, "Failed to validate framebuffer!");
        }
    }

    void ViewportPanel::OnUpdate(f32 Delta)
    {
        EditorWindow::OnUpdate(Delta);
        m_FrameBuffer->Resize(m_Size);
    }

    void ViewportPanel::OnInspectorGUI(const ImGuiIO& IO)
    {
        EditorWindow::OnInspectorGUI(IO);
        UI::ScopedStyleVarPusher WindowPadding(UI::StyleVar::WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::SetNextWindowSize(ImVec2(WindowStartSize.x, WindowStartSize.y), ImGuiCond_Appearing);
        UI::NewWindow(m_Name, m_Opened, WindowFlagBits::None, [this]
        {
            m_Size = ToVector2(ImGui::GetContentRegionAvail());
            m_Position = ToVector2(ImGui::GetWindowPos());
            m_MouseInside = ImGui::IsWindowHovered();
            const AttachmentTexture* ColorAttachment = m_FrameBuffer->GetAttachments().Single([](const AttachmentTexture& Attachment)
            {
                return Attachment.Attachment == FrameBufferAttachment::Color;
            });

            if (ColorAttachment && ColorAttachment->Texture)
            {
                UI::Image(ColorAttachment->Texture, m_Size);
            }
            
            m_DrawList = ImGui::GetWindowDrawList();
            if(!m_Available) m_Available = true;
        });
    }

    bool ViewportPanel::IsAvailable() const
    {
        return m_Available;
    }


    Vector2 ViewportPanel::GetSize() const
    {
        return m_Size;
    }

    FrameBuffer* ViewportPanel::GetFrameBuffer() const
    {
        return m_FrameBuffer;
    }

    ImDrawList* ViewportPanel::GetDrawList() const
    {
        return m_DrawList;
    }

    bool ViewportPanel::IsMouseHoveringWindow()
    {
        return g_Application->GetViewportPanel()->m_MouseInside;
    }
}
