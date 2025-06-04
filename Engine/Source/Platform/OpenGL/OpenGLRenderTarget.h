#pragma once
#include "Rendering/RenderTarget.h"

typedef Nova::u32 GLuint;

namespace Nova
{
    class OpenGLRenderTarget : public RenderTarget
    {
    public:
        explicit OpenGLRenderTarget(Renderer* Owner);

        bool Initialize(const RenderTargetCreateInfo& Info) override;
        bool Resize(const u32 Width, const u32 Height) override;
        void Destroy() override;

    private:
        GLuint m_ColorTexture = U32_MAX;
        GLuint m_DepthTexture = U32_MAX;
    };
}
