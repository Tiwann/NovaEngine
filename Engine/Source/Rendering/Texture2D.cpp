#include "Texture2D.h"

#include "Containers/ScopedPointer.h"
#include "Runtime/Assertion.h"
#include "Platform/PlatformTexture2D.h"
#include "Runtime/Log.h"
#include "Runtime/Sprite.h"
#include "Runtime/SpriteAnimation.h"
#include "Runtime/SpriteSheet.h"
#include "Rendering/Renderer.h"
#include "Math/Vector2.h"

namespace Nova
{
    Texture2D::Texture2D(const String& Name, const u32 Width, const u32 Height, const TextureParams& Params, const u32 Slot) : Asset(Name),
        m_Width(Width), m_Height(Height), m_Slot(Slot), m_Params(Params)
    {
        
    }


    Texture2D* Texture2D::Create(const String& Name, const GraphicsApi& GraphicsApi)
    {
        switch (GraphicsApi)
        {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return new OpenGLTexture2D(Name, 0, 0, TextureParams(), 0);
        case GraphicsApi::Vulkan: return new VulkanTexture2D(Name, 0, 0, TextureParams(), 0);
        case GraphicsApi::D3D12: return new D3D12Texture2D(Name, 0, 0, TextureParams(), 0);
        default: return nullptr;
        }
    }

    Texture2D* Texture2D::Create(const String& Name, const u32 Width, const u32 Height, const TextureParams& Params, const u32 Slot, const GraphicsApi& GraphicsApi)
    {
        switch (GraphicsApi)
        {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return new OpenGLTexture2D(Name, Width, Height, Params, Slot);
        case GraphicsApi::Vulkan: return new VulkanTexture2D(Name, Width, Height, Params, Slot);
        case GraphicsApi::D3D12: return new D3D12Texture2D(Name, Width, Height, Params, Slot);
        default: return nullptr;
        }
    }

    Texture2D* Texture2D::CreateFromFile(const String& Name, const Path& Filepath, const TextureParams& Params, const u32 Slot, const GraphicsApi& GraphicsApi)
    {
        Texture2D* Texture = Create(Name, 0, 0, Params, Slot, GraphicsApi);
        const ScopedBuffer RawImageData = File::ReadToBuffer(Filepath);
        const SharedPtr<Image> ImageData = MakeShared<Image>(BufferView(RawImageData.AsBuffer()), Format::R8G8B8A8_UNORM);
        Texture->SetData(ImageData);
        return Texture;
    }

    String Texture2D::GetAssetType() const
    {
        return "Texture2D";
    }

    void Texture2D::SetData(const SharedPtr<Image>& Image)
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
    
    Format Texture2D::GetFormat() const
    {
        return m_Params.Format;
    }

    void Texture2D::SetSlot(const u32 Slot)
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

    SpriteAnimation* Texture2D::CreateAnimation(const u32 NumRows, const u32 NumColumns, const u32 NumSprites, const u32 SpriteSize)
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
