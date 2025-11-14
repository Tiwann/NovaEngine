#pragma once
#include "Sprite.h"
#include "SpriteSheet.h"
#include "Asset.h"
#include "Containers/Array.h"

namespace Nova
{
    class SpriteAnimation : public Asset
    {
    public:
        SpriteAnimation() = default;
        AssetType GetAssetType() const override;

        bool LoadFromSpriteSheet(const SpriteSheet& spriteSheet);
        
        Array<Sprite>::SizeType Count() const;
        Sprite& GetSprite(size_t index);

        void SetSpriteSheet(const SpriteSheet& spriteSheet);
        SpriteSheet GetSpriteSheet() const;
    private:
        Array<Sprite> m_Sprites;
        SpriteSheet m_SpriteSheet;
    };
}
