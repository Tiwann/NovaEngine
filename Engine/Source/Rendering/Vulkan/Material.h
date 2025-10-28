#pragma once
#include "Rendering\Material.h"

namespace Nova::Vulkan
{
    class Material : public Rendering::Material
    {
    public:
        Material(const Ref<Rendering::Shader>& shader): Rendering::Material(shader){}

        bool Build() override;
        void BindTexture(StringView name, Ref<Rendering::Texture> texture) override;
        void BindBuffer(StringView name, Ref<Rendering::Buffer> buffer) override;
    };
}