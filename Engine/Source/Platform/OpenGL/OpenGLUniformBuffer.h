#pragma once
#include "Rendering/UniformBuffer.h"
#include "Runtime/Types.h"

typedef Nova::u32 GLuint;

namespace Nova
{
    class OpenGLUniformBuffer : public UniformBuffer
    {
    public:
        OpenGLUniformBuffer(Renderer* Owner);
        bool Allocate(size_t Size) override;
        void Free() override;
        void Copy(const void* Dest, size_t DestSize, size_t Offset) override;
    private:
        GLuint m_Handle = 0;
    };
}
