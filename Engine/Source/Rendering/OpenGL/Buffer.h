#pragma once
#include "Rendering/Buffer.h"

namespace Nova::OpenGL
{
    class RenderDevice;

    class Buffer final : public Nova::Buffer
    {
    public:
        Buffer() = default;

        bool Initialize(const BufferCreateInfo& createInfo) override;
        void Destroy() override;
        uint32_t GetHandle() const { return m_Handle; }

        void* Map() override;
        void Unmap(const void* ptr) override;
    private:
        RenderDevice* m_Device = nullptr;
        uint32_t m_Handle = 0xFFFFFFFF;
    };
}
