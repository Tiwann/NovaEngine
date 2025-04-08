#pragma once
#include "Runtime/Component.h"
#include "Runtime/Color.h"
#include "Runtime/Flags.h"
#include "Math/Vector2.h"
#include "Runtime/Sprite.h"
#include "Runtime/LogCategory.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(SpriteRenderer, "SPRITE RENDERER")

namespace Nova
{
    class VertexBuffer;
    class VertexArray;
    class IndexBuffer;
    class Shader;
    class Texture2D;

    enum class SpriteRendererFlagBits
    {
        None = 0,
        NOVA_FLAG_BIT(TileWithScale, 0),
        NOVA_FLAG_BIT(FlipHorizontal, 1),
        NOVA_FLAG_BIT(FlipVertical, 2),
        All = TileWithScale | FlipHorizontal | FlipVertical
    };

    NOVA_DECLARE_FLAGS(SpriteRendererFlagBits, SpriteRendererFlags);
    
    class SpriteRenderer : public Component
    {
    public:
        SpriteRenderer(Entity* Owner);
        
        SpriteRendererFlags Flags;
        
        void OnInit() override;
        void OnDestroy() override;
        void OnRender(Renderer* Renderer) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;
        
        /* Sprite  */
        Sprite GetSprite() const;
        void SetSprite(const Sprite& sprite);

        /* Tiling */
        void SetTiling(const Vector2& Tiling);
        Vector2 GetTiling() const;

        /* Size */
        void SetSize(const Vector2& Size);
        Vector2 GetSize() const;

        /* Color Tint */
        Color GetColorTint() const;
        void SetColorTint(const Color& Color);

        /* Pixels per unit */
        i32 GetPixelsPerUnit() const;
        void SetPixelsPerUnit(i32 PixelsPerUnit);

    private:
        VertexBuffer* m_VertexBuffer = nullptr;
        VertexArray* m_VertexArray = nullptr;
        IndexBuffer* m_IndexBuffer = nullptr;
        Shader* m_Shader = nullptr;
        Sprite m_Sprite;

        i32 m_PixelsPerUnit = 100;
        Vector2 m_Tiling{Vector2::One};
        Vector2 m_Size;
        Color m_ColorTint{Color::White};
    };
}
