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

    void SpriteAnimation::AddSprite(const Sprite& sprite)
    {
        m_Sprites.Add(sprite);
    }

    size_t SpriteAnimation::Count() const
    {
        return m_Sprites.Count();
    }

    Sprite& SpriteAnimation::GetSprite(size_t index)
    {
        return m_Sprites[index];
    }
}
