#pragma once
#include "Containers/Map.h"
#include "Rendering/ImGuiRenderer.h"

namespace Nova
{
    class Texture;
    class Sampler;
}

namespace Nova::Vulkan
{
    class RenderDevice;


    class ImGuiRenderer final : public Nova::ImGuiRenderer
    {
    public:
        bool Initialize(const ImGuiRendererCreateInfo& createInfo) override;
        void Destroy() override;
        void BeginFrame() override;
        void EndFrame() override;
        void Render(CommandBuffer& commandBuffer) override;

        void DrawTexture(const Nova::Texture& texture, uint32_t width, uint32_t height) override;
        uint64_t AddTexture(const Texture& texture);
        uint64_t GetOrAddTexture(const Texture& texture);

    private:
        RenderDevice* m_Device = nullptr;
        Map<const Texture*, uint64_t> m_Textures;
        Ref<Sampler> m_Sampler = nullptr;
    };
}
