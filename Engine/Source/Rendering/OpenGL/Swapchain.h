#pragma once
#include "Rendering/Swapchain.h"
#include "Common.h"

namespace Nova::OpenGL
{
    class Swapchain final : public Nova::Swapchain
    {
    public:
        bool Initialize(const SwapchainCreateInfo& createInfo) override;
        void Destroy() override;
        bool Recreate() override;
        void SetName(StringView name) override;
        Ref<Nova::Texture> GetTexture(uint32_t index) override;
        Ref<Nova::Texture> GetCurrentTexture() override;
    private:
        uint32_t m_Textures[3] { INVALID_HANDLE<uint32_t>, INVALID_HANDLE<uint32_t>, INVALID_HANDLE<uint32_t> };
    };
}
