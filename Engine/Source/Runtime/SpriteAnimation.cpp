#include "SpriteAnimation.h"

namespace Nova
{
    AssetType SpriteAnimation::GetAssetType() const
    {
        return AssetType::SpriteAnimation;
    }

    bool SpriteAnimation::LoadFromSpriteSheet(const SpriteSheet& spriteSheet)
    {
        if (!spriteSheet.texture) return false;
        if (spriteSheet.rows == 0 || spriteSheet.columns == 0) return false;
        if (spriteSheet.spriteCount <= 0) return false;
        if (spriteSheet.spriteSize <= 0) return false;

        m_Sprites.Clear();

        uint32_t processed = 0;
        for(uint32_t row = 0; row < spriteSheet.rows; row++)
        {
            if(processed >= spriteSheet.spriteCount) break;
            for(uint32_t column = 0; column < spriteSheet.columns; column++)
            {
                if(processed >= spriteSheet.spriteCount) break;

                Sprite sprite;
                sprite.x = column * spriteSheet.spriteSize;
                sprite.y = row * spriteSheet.spriteSize;
                sprite.width = spriteSheet.spriteSize;
                sprite.height = spriteSheet.spriteSize;
                sprite.texture = spriteSheet.texture;
                m_Sprites.Add(sprite);
                processed++;
            }
        }

        m_SpriteSheet = spriteSheet;
        return true;
    }

    size_t SpriteAnimation::Count() const
    {
        return m_Sprites.Count();
    }

    Sprite& SpriteAnimation::GetSprite(const size_t index)
    {
        return m_Sprites[index];
    }

    void SpriteAnimation::SetSpriteSheet(const SpriteSheet& spriteSheet)
    {
        m_SpriteSheet = spriteSheet;
    }

    SpriteSheet SpriteAnimation::GetSpriteSheet() const
    {
        return m_SpriteSheet;
    }
}
