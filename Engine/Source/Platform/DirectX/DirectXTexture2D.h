#pragma once
#include "Core/Graphics/Texture2D.h"

struct ID3D12Resource;

namespace Nova
{
    class DirectXTexture2D : public Texture2D
    {
    public:
        DirectXTexture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot);

        void SetTextureParameters(const TextureParams& Params) override;
        void SetData(u8* Data, u32 Width, u32 Height, ImageFormat Format) override;
        void SetData(const Ref<Image>& Image) override;
        Ref<Image> GetImage() const override;
        void Bind() const override;
        void Unbind() const override;
        uintptr_t GetHandle() const override;
        bool GetPixels(Buffer<u8>& OutPixels) const override;
    private:
        ID3D12Resource* m_Resource = nullptr;
    };
}
