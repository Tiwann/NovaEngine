#pragma once
#include "Renderer.h"
#include "Runtime/Asset.h"
#include "Runtime/Image.h"
#include "Runtime/LogCategory.h"
#include "Runtime/SpriteSheet.h"
#include "Filter.h"
#include "SamplerAddressMode.h"


NOVA_DECLARE_LOG_CATEGORY_STATIC(Texture2D, "TEXTURE2D");

namespace Nova
{
    class Image;
    struct Vector2;
    class Sprite;
    class SpriteAnimation;
    struct SpriteSheet;

    struct TextureParams
    {
        Filter Filter;
        SamplerAddressMode AddressMode;
        Format Format;
    };
    
    class Texture2D : public Asset
    {
    public:
        ~Texture2D() override = default;
        
        static Texture2D* Create(const String& Name, const GraphicsApi& GraphicsApi);
        static Texture2D* Create(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot, const GraphicsApi& GraphicsApi);
        static Texture2D* CreateFromFile(const String& Name, const Path& Filepath, const TextureParams& Params,u32 Slot, const GraphicsApi& GraphicsApi);

        String GetAssetType() const override;
        virtual void SetTextureParameters(const TextureParams& Params) = 0;
        virtual void SetData(u8* Data, u32 Width, u32 Height, const Format& Format) = 0;
        void SetData(const std::shared_ptr<Image>& Image);

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual uintptr_t GetHandle() const = 0;

        Vector2 GetSize() const;
        u32 GetWidth() const;
        u32 GetHeight() const;
        u32 GetSlot() const;
        Format GetFormat() const;
        void SetSlot(u32 Slot);

        Sprite CreateSprite(const Vector2& Position, const Vector2& Size);
        Sprite CreateSprite();
        SpriteAnimation* CreateAnimation(u32 NumRows, u32 NumColumns, u32 NumSprites, u32 SpriteSize);
        SpriteAnimation* CreateAnimation(const SpriteSheet& SpriteSheet);
    protected:
        Texture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot = 0);

        u32 m_Width = 0, m_Height = 0;
        u32 m_Slot = 0;
        TextureParams m_Params;
    };
}
