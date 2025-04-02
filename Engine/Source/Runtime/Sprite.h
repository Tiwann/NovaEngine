#pragma once
#include "Math/Vector2.h"
#include "SharedPointer.h"

namespace Nova
{
    class Texture2D;
    
    class Sprite
    {
    public:
        Sprite() : m_Position(Vector2::Zero), m_Size(Vector2::Zero), m_Texture(nullptr) {}
        Sprite(Texture2D* Texture, Vector2 Pos, Vector2 Size)
            : m_Position(Pos), m_Size(Size), m_Texture(Texture)
        {
            
        }

        Vector2 GetPosition() const;
        void SetPosition(const Vector2& position);
        
        Vector2 GetSize() const;
        void SetSize(const Vector2& size);
        
        Texture2D* GetTexture() const;
        void SetTexture(Texture2D* texture);

        static Sprite CreateFromTexture(Texture2D* Texture);
        static Sprite CreateFromTexture(Texture2D* Texture, const Vector2& Pos, const Vector2& Size);
    private:
        Vector2 m_Position;
        Vector2 m_Size;
        Texture2D* m_Texture;
    };
}
