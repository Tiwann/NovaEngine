#include "SpriteAnimation.h"
#include "Sprite.h"
#include "Rendering/Texture2D.h"

namespace Nova
{
    String SpriteAnimation::GetAssetType() const
    {
        return "SpriteAnimation";
    }

    SpriteAnimation* SpriteAnimation::Create()
    {
        return new SpriteAnimation;
    }

    SpriteAnimation* SpriteAnimation::CreateFromFiles(const Path* Filepaths, size_t Count)
    {
        SpriteAnimation* Result = Create();
        for(size_t i = 0; i < Count; i++)
        {
            const Path& Filepath = Filepaths[i];
            constexpr TextureParams Params = { Filter::Nearest, SamplerAddressMode::ClampToEdge };
            Texture2D* Texture = Texture2D::CreateFromFile("RuntimeLoadedTextureForSpriteAnim", Filepath, Params);
            Result->AddSprite(Sprite::CreateFromTexture(Texture));
        }
        return Result;
    }

    SpriteAnimation* SpriteAnimation::CreateFromDirectory(const Path& Directory)
    {
        const Array<Path> Filepaths = Directory::GetFiles(Directory);
        return CreateFromFiles(Filepaths.Data(), Filepaths.Count());
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

    void SpriteAnimation::AddNotify(const AnimationNotify& Notify)
    {
        m_Notifies.Add(Notify);
    }

    const Array<AnimationNotify>& SpriteAnimation::GetNotifies() const
    {
        return m_Notifies;
    }
}
