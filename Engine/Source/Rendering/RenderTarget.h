#pragma once
#include "Runtime/Format.h"
#include "Runtime/Object.h"
#include <cstdint>

#include "Runtime/Ref.h"


namespace Nova { struct Color; }

namespace Nova
{
    class Device;
    class CommandBuffer;
    class Texture;

    struct RenderTargetCreateInfo
    {
        Device* device = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 0;
        Format colorFormat = Format::None;
        Format depthFormat = Format::None;
        uint32_t sampleCount = 0;
    };

    class RenderTarget : public Object
    {
    public:
        RenderTarget() : Object("Render Target") {}
        ~RenderTarget() override = default;

        virtual bool Initialize(const RenderTargetCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        virtual bool Resize(uint32_t newX, uint32_t newY) = 0;

        virtual const Texture& GetColorTexture() = 0;
        virtual const Texture& GetDepthTexture() = 0;

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        uint32_t GetDepth() const;
        Format GetColorFormat() const;
        Format GetDepthFormat() const;
        uint32_t GetSampleCount() const;
        uint32_t GetImageCount() const;
    protected:
        Device* m_Device = nullptr;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_Depth = 0;
        Format m_ColorFormat = Format::None;
        Format m_DepthFormat = Format::None;
        uint32_t m_SampleCount = 0;
        uint32_t m_ImageCount = 0;
    };
}
