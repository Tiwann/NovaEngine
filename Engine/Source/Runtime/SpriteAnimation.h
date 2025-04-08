#pragma once
#include "Filesystem.h"
#include "Containers/Array.h"
#include "AnimationNotify.h"
#include "Asset.h"

namespace Nova
{
    class Sprite;
    
    class SpriteAnimation : public Asset
    {
    public:
        SpriteAnimation() = default;
        String GetAssetType() const override;

        static SpriteAnimation* Create();
        static SpriteAnimation* CreateFromFiles(const Path* Filepaths, size_t Count);
        static SpriteAnimation* CreateFromDirectory(const Path& Directory);
        void ClearSprites();
        void AddSprite(const Sprite& Sprite);
        
        Array<Sprite>::SizeType Count() const;
        Sprite& GetSprite(size_t Index);


        void AddNotify(const AnimationNotify& Notify);
        const Array<AnimationNotify>& GetNotifies() const;
    private:
        Array<Sprite> m_Sprites;
        Array<AnimationNotify> m_Notifies;
    };
}
