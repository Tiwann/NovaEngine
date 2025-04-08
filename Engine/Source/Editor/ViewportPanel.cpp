#include "ViewportPanel.h"
#include "EditorGUI.h"
#include "StyleVar.h"
#include "Runtime/Application.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture2D.h"

namespace
{
    inline const Nova::Vector2 WindowStartSize(960, 540);
}

namespace Nova
{
    void ViewportPanel::OnInit()
    {
        EditorWindow::OnInit();

        m_FrameBuffer = FrameBuffer::Create();
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
        
        UI::ScopedStyleVarPusher WindowPadding(UI::StyleVar::WindowPadding, Vector2::Zero);
        ImGui::SetNextWindowSize(WindowStartSize, ImGuiCond_Appearing);
        UI::NewWindow(m_Name, m_Opened, WindowFlagBits::None, [this]
        {
            m_Size = ImGui::GetContentRegionAvail();
            m_Position = ImGui::GetWindowPos();
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
