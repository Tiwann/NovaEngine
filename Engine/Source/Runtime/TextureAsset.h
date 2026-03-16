#pragma once
#include "Asset.h"
#include "Containers/StringView.h"
#include "Rendering/Texture.h"

namespace Nova
{
    class Stream;

    class TextureAsset final : public Asset
    {
    public:
        ~TextureAsset() override;
        AssetType GetAssetType() const override;

        bool LoadFromMemory(const uint8_t* data, size_t dataSize);
        bool LoadFromFile(StringView filepath);
        bool LoadFromStream(Stream& stream);

        bool IsValid() const;
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        uint32_t GetDepth() const;
        uint32_t GetMipCount() const;
        uint32_t GetArrayCount() const;
        Format GetFormat() const;
        TextureDimension GetDimension() const;

        WeakRef<Texture> GetTexture();
    private:
        Ref<Texture> m_Texture = nullptr;
    };
}
