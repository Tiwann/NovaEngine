#include "Texture2D.h"

#include "Runtime/Assertion.h"
#include "Platform/PlatformTexture2D.h"
#include "Runtime/Log.h"
#include "Runtime/Sprite.h"
#include "Runtime/SpriteAnimation.h"
#include "Runtime/SpriteSheet.h"

#include "Math/Vector2.h"

namespace Nova
{
    Texture2D::Texture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot) : Asset(Name),
        m_Width(Width), m_Height(Height), m_Slot(Slot), m_Params(Params)
    {
        
    }


    Texture2D* Texture2D::Create(const String& Name)
    {
        NOVA_RHI_PLATFORM_RETURN(Texture2D, Name, 0, 0, TextureParams().WithFilter(TextureFilter::Linear).WithWrap(TextureWrap::Clamp), 0);
    }

    Texture2D* Texture2D::Create(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot)
    {
        NOVA_RHI_PLATFORM_RETURN(Texture2D, Name, Width, Height, Params, Slot);
    }

    Texture2D* Texture2D::CreateFromFile(const String& Name, const Path& Filepath, const TextureParams& Params,u32 Slot)
    {
        Texture2D* Texture = Create(Name, 0, 0, Params, Slot);
        const ScopedBuffer<u8> RawImageData = File::ReadToBuffer(Filepath);
        const Ref<Image> ImageData = Image::Create(BufferView<u8>(RawImageData.AsBuffer()), ImageFormat::RGBA8);
        Texture->SetData(ImageData);
        return Texture;
    }

    Texture2D* Texture2D::CreateWhiteTexture(u32 Width, u32 Height)
    {
        constexpr TextureParams Params = {.Filter = TextureFilter::Nearest, .Wrap = TextureWrap::Clamp};
        Texture2D* Texture = Create("WhiteTexture", Width, Height, Params);
        const size_t Size = (size_t)Width * (size_t)Height * 4ULL;
        u8* Data = new u8[Size]{};
        memset(Data, 0xffui8, Size);
        Texture->SetData(Data, Width, Height, ImageFormat::RGBA8);
        delete[] Data;
        return Texture;
    }

    String Texture2D::GetAssetType() const
    {
        return "Texture2D";
    }

    void Texture2D::SetData(const Ref<Image>& Image)
    {
        SetData((u8*)Image->GetData(), Image->GetWidth(), Image->GetHeight(), Image->GetFormat());
    }

    Vector2 Texture2D::GetSize() const
    {
        return {(f32)m_Width, (f32)m_Height};
    }

    u32 Texture2D::GetWidth() const
    {
        return m_Width;
    }

    u32 Texture2D::GetHeight() const
    {
        return m_Height;
    }

    u32 Texture2D::GetSlot() const
    {
        return m_Slot;
    }
    
    ImageFormat Texture2D::GetFormat() const
    {
        return m_Format;
    }

    void Texture2D::SetSlot(u32 Slot)
    {
        m_Slot = Slot;
    }

    Sprite Texture2D::CreateSprite(const Vector2& Position, const Vector2& Size)
    {
        NOVA_ASSERT(Position.x >= 0 && Position.x < (f32)m_Width &&
                    Position.y >= 0 && Position.y < (f32)m_Height &&
                    Position.x + Size.x <= (f32)m_Width && 
                    Position.y + Size.y <= (f32)m_Height, "Failed to created sprite");

        return { this, Position, Size };
    }

    Sprite Texture2D::CreateSprite()
    {
        return { this, Vector2::Zero, GetSize() };
    }

    SpriteAnimation* Texture2D::CreateAnimation(u32 NumRows, u32 NumColumns, u32 NumSprites, u32 SpriteSize)
    {
        SpriteAnimation* Result = SpriteAnimation::Create();

        u32 Processed = 0;
        for(u32 Row = 0; Row < NumRows; Row++)
        {
            if(Processed >= NumSprites) break;
            for(u32 Column = 0; Column < NumColumns; Column++)
            {
                if(Processed >= NumSprites) break;
                const Vector2 Position = {(f32)(Column * SpriteSize), (f32)(Row * SpriteSize)};
                const Vector2 Size = Vector2::One * (f32)SpriteSize;
                Sprite Sprite = CreateSprite(Position, Size);
                Result->AddSprite(Sprite);
                Processed++;
            }
        }
        return Result;
    }

    SpriteAnimation* Texture2D::CreateAnimation(const SpriteSheet& SpriteSheet)
    {
        return CreateAnimation(SpriteSheet.NumRows,
                               SpriteSheet.NumColumns,
                               SpriteSheet.NumSprites,
                               SpriteSheet.SpriteSize);
    }
}
