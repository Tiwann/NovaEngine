#pragma once
#include "Runtime/Format.h"
#include <cstdint>

namespace Nova { struct Color; }

namespace Nova::Rendering
{
    class Device;
    class CommandBuffer;

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

    class RenderTarget
    {
    public:
        RenderTarget() = default;
        virtual ~RenderTarget() = default;

        virtual bool Initialize(const RenderTargetCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        virtual void BeginRendering(CommandBuffer& commandBuffer) = 0;
        virtual void EndRendering() = 0;

        virtual bool Resize(uint32_t newX, uint32_t newY) = 0;
        virtual void Clear(const Color& color) = 0;
        virtual void Clear(float depth, uint8_t stencil) = 0;

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetDepth() const { return m_Depth; }
        Format GetColorFormat() const { return m_ColorFormat; }
        Format GetDepthFormat() const { return m_DepthFormat; }
        uint32_t GetSampleCount() const { return m_SampleCount; }
    protected:
        Device* m_Device = nullptr;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_Depth = 0;
        Format m_ColorFormat = Format::None;
        Format m_DepthFormat = Format::None;
        uint32_t m_SampleCount = 0;
    };
}
