#pragma once
#include "Rendering/Texture2D.h"

struct ID3D12Resource;

namespace Nova
{
    class D3D12Texture2D : public Texture2D
    {
    public:
        D3D12Texture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot);

        void SetTextureParameters(const TextureParams& Params) override;
        void SetData(u8* Data, u32 Width, u32 Height, const Formats& Format) override;
        SharedPtr<Image> GetImage() const override;
        void Bind() const override;
        void Unbind() const override;
        uintptr_t GetHandle() const override;
    private:
        ID3D12Resource* m_Resource = nullptr;
    };
}
