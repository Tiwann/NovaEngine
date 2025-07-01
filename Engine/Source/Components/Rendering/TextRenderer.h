#pragma once
#include "Containers/Lazy.h"
#include "Runtime/Component.h"
#include "Runtime/Flags.h"

namespace Nova
{
    class Font;
}

namespace Nova
{
    enum class TextAlignment
    {
        Left,
        Center,
        Right
    };

    enum class TextStyleFlagBits
    {
        None = 0,
        Bold = BIT(0),
        Italic = BIT(1),
        Underline = BIT(2),
    };

    NOVA_DECLARE_FLAGS(TextStyleFlagBits, TextStyleFlags)


    class TextRenderer : public Component
    {
    public:
        explicit TextRenderer(Entity* Owner) : Component(Owner, "Text Renderer")
        {
        }

        void OnInit() override;
        void OnRender(Renderer* Renderer) override;

        void OnInspectorGUI(const ImGuiIO& IO) override;

        void SetText(const String& NewText);
        const String& GetText() const;

        void SetFont(Font* Font);
        Font* GetFont() const;

        void SetTextAlignment(TextAlignment NewTextAlignment);
        TextAlignment GetTextAlignment() const;

        void SetTextStyle(TextStyleFlags NewTextStyleFlags);
        TextStyleFlags GetTextStyle() const;
    private:
        String m_Text;
        Font* m_Font = nullptr;
        TextStyleFlags m_StyleFlags = TextStyleFlagBits::None;
        TextAlignment m_TextAlignment = TextAlignment::Left;
        Lazy<class Pipeline*> m_Pipeline = nullptr;
        class VertexBuffer* m_VertexBuffer = nullptr;
        class IndexBuffer* m_IndexBuffer = nullptr;
    };
}
