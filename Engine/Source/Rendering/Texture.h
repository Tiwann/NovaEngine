#pragma once
#include "Runtime/Format.h"
#include "TextureUsage.h"
#include "ResourceState.h"
#include "Runtime/Asset.h"
#include <cstdint>


namespace Nova
{
    class Device;

    struct TextureCreateInfo
    {
        Device* device = nullptr;
        TextureUsageFlags usageFlags = TextureUsageFlagBits::None;
        Format format = Format::None;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 1;
        uint32_t mips = 1;
        uint32_t sampleCount = 0;
        const uint8_t* data = nullptr;
        size_t dataSize = 0;

        TextureCreateInfo& WithDevice(Device* inDevice) { device = inDevice; return *this; }
        TextureCreateInfo& WithUsageFlags(TextureUsageFlags inFlags) { usageFlags = inFlags; return *this; }
        TextureCreateInfo& WithFormat(const Format inFormat) { format = inFormat; return *this; }
        TextureCreateInfo& WithWidth(const uint32_t inWidth) { width = inWidth; return *this; }
        TextureCreateInfo& WithHeight(const uint32_t inHeight) { height = inHeight; return *this; }
        TextureCreateInfo& WithDepth(const uint32_t inDepth) { depth = inDepth; return *this; }
        TextureCreateInfo& WithMips(const uint32_t inMips) { mips = inMips; return *this; }
        TextureCreateInfo& WithSampleCount(const uint32_t inSampleCount) { sampleCount = inSampleCount; return *this; }
        TextureCreateInfo& WithData(const uint8_t* inData, const size_t inSize) { data = inData; dataSize = inSize; return *this; }
        TextureCreateInfo& WithData(const Array<uint8_t>& inData) { data = inData.Data(); dataSize = inData.Size(); return *this; }
    };

    class Texture : public Asset
    {
    public:
        Texture() : Asset("Texture") {}
        ~Texture() override = default;

        virtual bool Initialize(const TextureCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        virtual bool IsValid() = 0;
        virtual Array<uint8_t> GetPixels() = 0;

        AssetType GetAssetType() const final { return AssetType::Texture; }

        Format GetFormat() const { return m_Format; }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetMips() const { return m_Mips; }
        uint32_t GetSampleCount() const { return m_SampleCount; }
        ResourceState GetState() const { return m_State; }
        void SetState(const ResourceState state) { m_State = state; }
        TextureUsageFlags GetusageFlags() const { return m_UsageFlags; }
    protected:
        Format m_Format = Format::None;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_Depth = 0;
        uint32_t m_Mips = 0;
        uint32_t m_SampleCount = 0;
        ResourceState m_State = ResourceState::Undefined;
        TextureUsageFlags m_UsageFlags = TextureUsageFlagBits::None;
    };
}
