#include "SpriteAnimation.h"

namespace Nova
{
    String SpriteAnimation::GetAssetType() const
    {
        return "Sprite Animation";
    }

    void SpriteAnimation::ClearSprites()
    {
        m_Sprites.Clear();
    }

    void SpriteAnimation::AddSprite(const Sprite& Sprite)
    {
        m_Sprites.Add(Sprite);
    }

    size_t SpriteAnimation::Count() const
    {
        return m_Sprites.Count();
    }

    Sprite& SpriteAnimation::GetSprite(size_t Index)
    {
        return m_Sprites[Index];
    }
}
