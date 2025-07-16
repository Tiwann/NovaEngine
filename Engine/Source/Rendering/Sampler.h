#pragma once
#include "Runtime/Format.h"
#include <cstdint>
#include <format>

namespace Nova::Rendering
{
    class Device;

    struct TextureCreateInfo
    {
        Device* device;
        Format format;
        uint32_t width;
        uint32_t height;
        uint32_t mips;
        uint32_t sampleCount;
        uint8_t* data;
        size_t dataSize;
    };

    class Texture
    {
    public:
        Texture() = default;
        virtual ~Texture() = default;

        virtual bool Initialize(const TextureCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        virtual bool IsValid() = 0;

        Format GetFormat() const { return m_Format; }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetMips() const { return m_Mips; }
        uint32_t GetSamples() const { return m_Samples; }
    protected:
        Format m_Format;
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_Mips;
        uint32_t m_Samples;
    };
}
