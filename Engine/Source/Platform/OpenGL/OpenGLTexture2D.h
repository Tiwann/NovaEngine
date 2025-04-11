#pragma once
#include "Rendering/Texture2D.h"

typedef Nova::i32 GLint;

namespace Nova
{
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot);
        ~OpenGLTexture2D() override;

        void SetTextureParameters(const TextureParams& Params) override;
        void SetData(u8* Data, u32 Width, u32 Height, const Formats& Format) override;
        SharedPtr<Image> GetImage() const override;
        void Bind() const override;
        void Unbind() const override;

        uintptr_t GetHandle() const override;

    private:
        u32 FormatToOpenGLFormat(Formats Format) const;
        u32 FormatToType(Formats Format) const;

    private:
        u32 m_Handle{UINT32_MAX};
        GLint GetTextureWrap(TextureWrap Wrap);
        GLint GetTextureFilter(TextureFilter Filter);
    };
}
