#pragma once
#include "TextureAspect.h"
#include "Runtime/Format.h"
#include "Runtime/Object.h"
#include <cstdint>

namespace Nova
{
    class RenderDevice;
    class Texture;

    struct TextureViewCreateInfo
    {
        RenderDevice* device = nullptr;
        const Texture* texture = nullptr;
        Format format = Format::None;
        TextureAspectFlags aspectFlags = TextureAspectFlagBits::None;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 0;
        uint32_t baseMipLevel = 0;
        uint32_t mipCount = 0;
        uint32_t baseArray = 0;
        uint32_t arrayCount = 0;
    };

    class TextureView : public Object
    {
    public:
        TextureView() = default;
        ~TextureView() override = default;
        virtual bool Initialize(const TextureViewCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        const Texture* GetTexture() const { return m_Texture; }
        Format GetFormat() const { return m_Format; }
        TextureAspectFlags GetAspectFlags() const { return m_AspectFlags; }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetDepth() const { return m_Depth; }
        uint32_t GetBaseMipLevel() const { return m_BaseMipLevel; }
        uint32_t GetMipCount() const { return m_MipCount; }
    protected:
        RenderDevice* m_Device = nullptr;
        const Texture* m_Texture = nullptr;
        Format m_Format = Format::None;
        TextureAspectFlags m_AspectFlags = TextureAspectFlagBits::None;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_Depth = 0;
        uint32_t m_BaseMipLevel = 0;
        uint32_t m_MipCount = 0;
    };
}
