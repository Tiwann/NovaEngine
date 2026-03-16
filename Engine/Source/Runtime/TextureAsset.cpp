#include "TextureAsset.h"
#include "Rendering/RenderDevice.h"
#include "Runtime/FileUtils.h"
#include "Utils/TextureUtils.h"

namespace Nova
{
    bool TextureAsset::LoadFromMemory(const uint8_t* data, const size_t dataSize)
    {
        Ref<RenderDevice> device = RenderDevice::GetInstance();
        if (!device) return false;
        Ref<Texture> loadedTexture = TextureUtils::LoadTexture(device, data, dataSize);
        if (!loadedTexture) return false;
        m_Texture = loadedTexture;
        return true;
    }

    bool TextureAsset::LoadFromFile(const StringView filepath)
    {
        Array<uint8_t> data = FileUtils::ReadToBuffer(filepath);
        if (data.IsEmpty()) return false;
        return LoadFromMemory(data.Data(), data.Size());
    }

    bool TextureAsset::LoadFromStream(Stream& stream)
    {
        Array<uint8_t> data = FileUtils::ReadTillEnd(stream);
        if (data.IsEmpty()) return false;
        return LoadFromMemory(data.Data(), data.Size());
    }

    TextureAsset::~TextureAsset()
    {
        m_Texture->Destroy();
        m_Texture = nullptr;
    }

    AssetType TextureAsset::GetAssetType() const
    {
        return AssetType::Texture;
    }

    bool TextureAsset::IsValid() const
    {
        return m_Texture;
    }

    uint32_t TextureAsset::GetWidth() const
    {
        return m_Texture->GetWidth();
    }

    uint32_t TextureAsset::GetHeight() const
    {
        return m_Texture->GetHeight();
    }

    uint32_t TextureAsset::GetDepth() const
    {
        return m_Texture->GetDepth();
    }

    uint32_t TextureAsset::GetMipCount() const
    {
        return m_Texture->GetMipCount();
    }

    uint32_t TextureAsset::GetArrayCount() const
    {
        return m_Texture->GetArrayCount();
    }

    Format TextureAsset::GetFormat() const
    {
        return m_Texture->GetFormat();
    }

    TextureDimension TextureAsset::GetDimension() const
    {
        return m_Texture->GetDimension();
    }

    WeakRef<Texture> TextureAsset::GetTexture()
    {
        return m_Texture;
    }
}
