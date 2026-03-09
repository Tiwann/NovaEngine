#pragma once
#include "Runtime/Format.h"
#include "Runtime/Object.h"
#include "Runtime/Ref.h"
#include <cstdint>


namespace Nova { struct Color; }

namespace Nova
{
    class RenderDevice;
    class CommandBuffer;
    class TextureView;
    class Texture;

    struct RenderTargetCreateInfo
    {
        RenderDevice* device = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
        Format colorFormat = Format::None;
        Format depthFormat = Format::None;
        uint32_t sampleCount = 0;
    };

    class RenderTarget final : public Object
    {
    public:
        RenderTarget() : Object("Render Target") {}
        ~RenderTarget() override = default;

        bool Initialize(const RenderTargetCreateInfo& createInfo);
        void Destroy();
        bool Resize(uint32_t newWidth, uint32_t newHeight);

        Ref<Texture> GetColorTexture();
        Ref<Texture> GetDepthTexture();
        Ref<TextureView> GetColorTextureView();
        Ref<TextureView> GetDepthTextureView();

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        Format GetColorFormat() const;
        Format GetDepthFormat() const;
        uint32_t GetSampleCount() const;
        uint32_t GetImageCount() const;
    protected:
        RenderDevice* m_Device = nullptr;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        Format m_ColorFormat = Format::None;
        Format m_DepthFormat = Format::None;
        uint32_t m_SampleCount = 0;
        uint32_t m_ImageCount = 0;

        Ref<Texture> m_ColorTextures[3] = { nullptr, nullptr, nullptr };
        Ref<Texture> m_DepthTextures[3] = { nullptr, nullptr, nullptr };
        Ref<TextureView> m_ColorTextureViews[3] = { nullptr, nullptr, nullptr };
        Ref<TextureView> m_DepthTextureViews[3] = { nullptr, nullptr, nullptr };
    };
}
