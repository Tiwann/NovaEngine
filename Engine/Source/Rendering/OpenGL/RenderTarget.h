#pragma once
#include "Rendering/RenderTarget.h"
#include "Common.h"

namespace Nova::OpenGL
{
    class RenderTarget final : public Nova::RenderTarget
    {
    public:
        bool Initialize(const RenderTargetCreateInfo& createInfo) override;
        void Destroy() override;
        bool Resize(uint32_t newX, uint32_t newY) override;
        Ref<Texture> GetColorTexture() override;
        Ref<Texture> GetDepthTexture() override;
    private:
        uint32_t m_FrameBuffers[3] { INVALID_HANDLE<uint32_t>, INVALID_HANDLE<uint32_t>, INVALID_HANDLE<uint32_t> };
        uint32_t m_ColorTextures[3] { INVALID_HANDLE<uint32_t>, INVALID_HANDLE<uint32_t>, INVALID_HANDLE<uint32_t> };
        uint32_t m_DepthTextures[3] { INVALID_HANDLE<uint32_t>, INVALID_HANDLE<uint32_t>, INVALID_HANDLE<uint32_t> };
    };
}