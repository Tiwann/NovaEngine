#pragma once
#include "Runtime/Component.h"
#include "Math/Vector4.h"

namespace Nova
{
    class Font;
}

namespace Nova
{
    enum class TextAlign
    {
        Left,
        Center,
        Right
    };
    
    class TextRenderer : public Component
    {
    public:
        TextRenderer(Entity* Owner) : Component(Owner, "Text Renderer")
        {
        }

        void OnInspectorGUI(const ImGuiIO& IO) override;

        String Text;
        float FontSize = 1.0f;
        Vector4 Margin;
        Vector4 Padding;
        
        void SetFont(Font* Font);
        Font* GetFont() const;
    private:
        Font* m_Font = nullptr;
    };
}
