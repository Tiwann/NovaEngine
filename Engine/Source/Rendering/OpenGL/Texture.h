#pragma once
#include "Rendering/Texture.h"

namespace Nova::OpenGL
{
    class Device;

    class Texture final : public Nova::Texture
    {
    public:
        Texture() = default;
        ~Texture() override = default;

        bool Initialize(const TextureCreateInfo& createInfo) override;
        void Destroy() override;
        bool IsValid() override;

        uint32_t GetHandle() const;
        Array<uint8_t> GetPixels() override;
    private:
        Device* m_Device = nullptr;
        uint32_t m_Handle = 0xFFFFFFFF;
    };
}
