#pragma once
#include "Runtime/Format.h"
#include "Runtime/Flags.h"
#include "Runtime/Asset.h"
#include <cstdint>


namespace Nova::Rendering
{
    class Device;

    enum class TextureUsageFlagBits
    {
        None = 0,
        Sampled,
        Storage,
        Attachment
    };

    typedef Flags<TextureUsageFlagBits> TextureUsageFlags;

    struct TextureCreateInfo
    {
        Device* device = nullptr;
        TextureUsageFlags usageFlags = TextureUsageFlagBits::None;
        Format format = Format::None;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 0;
        uint32_t mips = 0;
        uint32_t sampleCount = 0;
        uint8_t* data = nullptr;
        size_t dataSize = 0;
    };

    class Texture : public Asset
    {
    public:
        Texture() : Asset("Texture") {}
        ~Texture() override = default;

        virtual bool Initialize(const TextureCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        virtual bool IsValid() = 0;

        String GetAssetType() const override { return "Texture"; }

        Format GetFormat() const { return m_Format; }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetMips() const { return m_Mips; }
        uint32_t GetSampleCount() const { return m_SampleCount; }
        TextureUsageFlags GetUsageFlags() const { return m_UsageFlags; }
    protected:
        Format m_Format = Format::None;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_Depth = 0;
        uint32_t m_Mips = 0;
        uint32_t m_SampleCount = 0;
        TextureUsageFlags m_UsageFlags = TextureUsageFlagBits::None;
    };
}
