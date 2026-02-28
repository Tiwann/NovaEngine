#pragma once
#include "Rendering/ShaderModule.h"
#include "Common.h"

namespace Nova::OpenGL
{
    class ShaderModule final : public Nova::ShaderModule
    {
    public:
        bool Initialize(const ShaderModuleCreateInfo& createInfo) override;
        void Destroy() override;

        uint32_t GetHandle() const;

    private:
        uint32_t m_Handle = INVALID_HANDLE<uint32_t>;
    };
}
