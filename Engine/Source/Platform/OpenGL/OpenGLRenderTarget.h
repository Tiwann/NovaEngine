#pragma once
#include "Rendering/RenderTarget.h"

typedef Nova::u32 GLuint;
namespace Nova
{
    struct OpenGLRenderTargetAttachment : RenderTargetAttachment
    {
        GLuint Handle = U32_MAX;
    };

    class OpenGLRenderTarget : public RenderTarget
    {
    public:
        explicit OpenGLRenderTarget(Renderer* Owner);

        bool Initialize(const RenderTargetCreateInfo& Info) override;
        bool Resize(const u32 Width, const u32 Height) override;
        void Destroy() override;
        void BeginRendering() override;
        void EndRendering() override;

    private:
        GLuint m_Handle = U32_MAX;
    };
}
