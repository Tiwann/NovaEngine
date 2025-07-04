#pragma once
#include "Containers/Lazy.h"
#include "Rendering/Vertex.h"
#include "Runtime/Component.h"
#include "Runtime/Flags.h"

typedef struct VkDescriptorSet_T* VkDescriptorSet;

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
        void OnDestroy() override;
        void OnUpdate(f32 Delta) override;
        void OnFrameBegin(Renderer* Renderer) override;
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

        float CalculateTextWidth() const;
    private:
        void UpdateResources();
        void GetTextQuads(Array<Vertex>& OutVertices, Array<u32>& OutIndices);
    private:
        String m_Text = "Enter Text...";
        bool m_ResourcesDirty = false;
        Font* m_Font = nullptr;
        TextStyleFlags m_StyleFlags = TextStyleFlagBits::None;
        TextAlignment m_TextAlignment = TextAlignment::Left;
        f32 m_LineSpacing = 1.0f;
        f32 m_CharacterSpacing = 0.0f;
        class Pipeline* m_Pipeline = nullptr;
        class VertexBuffer* m_VertexBuffer = nullptr;
        class IndexBuffer* m_IndexBuffer = nullptr;
        class UniformBuffer* m_UniformBuffer = nullptr;
        Array<VkDescriptorSet> m_DescriptorSets;
    };
}
