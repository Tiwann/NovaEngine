#pragma once
#include "Sprite.h"
#include "Game/Asset.h"
#include "Containers/Array.h"

namespace Nova
{
    class SpriteAnimation : public Asset
    {
    public:
        SpriteAnimation() = default;
        String GetAssetType() const override;

        void ClearSprites();
        void AddSprite(const Sprite& Sprite);
        
        Array<Sprite>::SizeType Count() const;
        Sprite& GetSprite(size_t Index);
    private:
        Array<Sprite> m_Sprites;
    };
}
