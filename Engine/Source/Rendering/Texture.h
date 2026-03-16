#pragma once
#include "Resource.h"
#include "Runtime/Format.h"
#include "TextureUsage.h"
#include "TextureDimension.h"
#include "ResourceState.h"
#include "Runtime/Ref.h"
#include <cstdint>

#include "Math/Functions.h"
#include "Math/Rect.h"

namespace Nova
{
    class RenderDevice;
    class TextureView;

    struct TextureCreateInfo
    {
        RenderDevice* device = nullptr;
        TextureUsageFlags usageFlags = TextureUsageFlagBits::None;
        Format format = Format::None;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 1;
        uint32_t mipCount = 1;
        uint32_t sampleCount = 1;
        uint32_t arrayCount = 1;

        static TextureCreateInfo Texture1D(const uint32_t width, const Format format, const uint32_t mipCount = 1, const uint32_t sampleCount = 1)
        {
            TextureCreateInfo createInfo;
            createInfo.usageFlags = TextureUsageFlagBits::Default;
            createInfo.format = format;
            createInfo.width = width;
            createInfo.height = 1;
            createInfo.depth = 1;
            createInfo.mipCount = mipCount;
            createInfo.sampleCount = sampleCount;
            createInfo.arrayCount = 1;
            return createInfo;
        }

        static TextureCreateInfo Texture2D(const uint32_t width, const uint32_t height, const Format format, const uint32_t mipCount = 1, const uint32_t sampleCount = 1)
        {
            TextureCreateInfo createInfo;
            createInfo.usageFlags = TextureUsageFlagBits::Default;
            createInfo.format = format;
            createInfo.width = width;
            createInfo.height = height;
            createInfo.depth = 1;
            createInfo.mipCount = mipCount;
            createInfo.sampleCount = sampleCount;
            createInfo.arrayCount = 1;
            return createInfo;
        }

        static TextureCreateInfo Texture3D(const uint32_t width, const uint32_t height, const uint32_t depth, const Format format, const uint32_t mipCount = 1, const uint32_t sampleCount = 1)
        {
            TextureCreateInfo createInfo;
            createInfo.usageFlags = TextureUsageFlagBits::Default;
            createInfo.format = format;
            createInfo.width = width;
            createInfo.height = height;
            createInfo.depth = depth;
            createInfo.mipCount = mipCount;
            createInfo.sampleCount = sampleCount;
            createInfo.arrayCount = 1;
            return createInfo;
        }

    };

    class Texture : public Resource
    {
    public:
        Texture() : Resource(){}
        ~Texture() override = default;

        virtual bool Initialize(const TextureCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        virtual bool IsValid() = 0;

        ResourceType GetResourceType() final { return ResourceType::Texture; }
        Format GetFormat() const { return m_Format; }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetDepth() const { return m_Depth; }
        uint32_t GetMipCount() const { return m_Mips; }
        uint32_t GetSampleCount() const { return m_SampleCount; }
        ResourceState GetState() const { return m_State; }
        uint32_t GetArrayCount() const { return m_ArrayCount; }
        void SetState(const ResourceState state) { m_State = state; }
        TextureUsageFlags GetUsageFlags() const { return m_UsageFlags; }
        TextureDimension GetDimension() const { return m_Dimension; }
        void GetMipDimensions(const uint32_t mipLevel, uint32_t* width, uint32_t* height, uint32_t* depth)
        {
            if (width)
                *width = Math::Max(1u, m_Width >> mipLevel);
            if (height)
                *height = Math::Max(1u, m_Height >> mipLevel);
            if (depth)
                *depth = Math::Max(1u, m_Depth >> mipLevel);
        }
        const Ref<TextureView>& GetView() const { return m_View; }
    protected:
        Format m_Format = Format::None;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_Depth = 0;
        uint32_t m_Mips = 0;
        uint32_t m_SampleCount = 0;
        uint32_t m_ArrayCount = 0;
        ResourceState m_State = ResourceState::Undefined;
        TextureUsageFlags m_UsageFlags = TextureUsageFlagBits::None;
        TextureDimension m_Dimension = TextureDimension::None;
        Ref<TextureView> m_View = nullptr;
    };

    using TextureHandle = Ref<Texture>;


}
