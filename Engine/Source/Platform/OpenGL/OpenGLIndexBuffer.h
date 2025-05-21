#pragma once
#include "Rendering/IndexBuffer.h"

namespace Nova
{
    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        explicit OpenGLIndexBuffer(Renderer* Renderer);
        bool Initialize(const IndexBufferCreateInfo& CreateInfo) override;
        void Destroy() override;
        void SetDebugName(const String& Name) override;

    private:
        u32 m_Handle = U32_MAX;
    };
}
