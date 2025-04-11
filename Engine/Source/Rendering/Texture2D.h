#pragma once
#include "Renderer.h"
#include "Runtime/Asset.h"
#include "Runtime/Image.h"
#include "Runtime/SharedPointer.h"
#include "Runtime/LogCategory.h"
#include "Runtime/SpriteSheet.h"


NOVA_DECLARE_LOG_CATEGORY_STATIC(Texture2D, "TEXTURE2D");

namespace Nova
{
    class Image;
    struct Vector2;
    class Sprite;
    class SpriteAnimation;
    struct SpriteSheet;

    enum class TextureFilter
    {
        Nearest,
        Linear,
    };

    enum class TextureWrap
    {
        Clamp,
        Repeat,
        Mirror,
    };

    struct TextureParams
    {
        TextureFilter Filter;
        TextureWrap Wrap;
        Formats Format;
    };
    
    class Texture2D : public Asset
    {
    public:
        Texture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot = 0);
        ~Texture2D() override = default;


        static Texture2D* Create(const String& Name = "NewTexture", const GraphicsApi& GraphicsApi = {});
        static Texture2D* Create(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot = 0, const GraphicsApi& GraphicsApi =
                                     {});
        static Texture2D* CreateFromFile(const String& Name, const Path& Filepath, const TextureParams& Params,u32 Slot = 0);
        static Texture2D* CreateWhiteTexture(u32 Width, u32 Height);

        String GetAssetType() const override;
        virtual void SetTextureParameters(const TextureParams& Params) = 0;
        virtual void SetData(u8* Data, u32 Width, u32 Height, const Formats& Format) = 0;
        void SetData(const SharedPtr<Image>& Image);
        virtual SharedPtr<Image> GetImage() const = 0;
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual uintptr_t GetHandle() const = 0;

        Vector2 GetSize() const;
        u32 GetWidth() const;
        u32 GetHeight() const;
        u32 GetSlot() const;
        Formats GetFormat() const;
        void SetSlot(u32 Slot);

        template<typename T>
        T As() const { return (T)GetHandle(); }
        
        Sprite CreateSprite(const Vector2& Position, const Vector2& Size);
        Sprite CreateSprite();
        SpriteAnimation* CreateAnimation(u32 NumRows, u32 NumColumns, u32 NumSprites, u32 SpriteSize);
        SpriteAnimation* CreateAnimation(const SpriteSheet& SpriteSheet);
    protected:
        u32 m_Width{0}, m_Height{0};
        u32 m_Slot{0};
        Formats m_Format{Formats::NONE};
        TextureParams m_Params;
    };
}
