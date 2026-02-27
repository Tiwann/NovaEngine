#pragma once
#include "Runtime/Format.h"
#include "Runtime/Object.h"
#include "Runtime/Ref.h"

namespace Nova
{
    class Texture;
    class Shader;
    class GraphicsPipeline;

    enum class GBufferPassType
    {
        Albedo,
        Position,
        Normal,
        Tangent,
        ChannelCombinedCustom,
        Emission,
        Lighting,
    };

    struct GBufferDescription
    {
        uint32_t numPasses = 0;
        struct GBufferPassDescription* passesDescriptions = nullptr;
    };

    struct GBufferPassDescription
    {
        uint32_t passIndex;
        GBufferPassType type;
        Format format;
        Ref<Shader> shader = nullptr;
    };

    struct GBufferPass
    {
        GBufferPassDescription description;
        Ref<Texture> textures[3] = {nullptr, nullptr, nullptr};
        Ref<GraphicsPipeline> pipeline;
    };

    struct DeferredRendererCreateInfo
    {
        class RenderDevice* device = nullptr;
        class RenderTarget* renderTarget = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
        GBufferDescription gBufferDescription;
    };

    class DeferredRenderer final : public Object
    {
    public:
        DeferredRenderer() = default;
        ~DeferredRenderer() override = default;

        bool Initialize(const DeferredRendererCreateInfo& createInfo);
        void Destroy();
        void Resize(uint32_t width, uint32_t height);

        bool IsValid() const;
        void Invalidate();
    private:
        RenderDevice* m_Device = nullptr;
        RenderTarget* m_RenderTarget = nullptr;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        bool m_IsValid = false;
        GBufferDescription m_GBufferDescription;
        Array<GBufferPass> m_GBufferPasses;
    };
}