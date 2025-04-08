#include "TextRenderer.h"

namespace Nova
{
    void TextRenderer::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);
    }

    void TextRenderer::SetFont(Font* Font)
    {
        m_Font = Font;
    }

    Font* TextRenderer::GetFont() const
    {
        return m_Font;
    }

}