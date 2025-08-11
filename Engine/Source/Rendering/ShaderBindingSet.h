#pragma once

namespace Nova::Rendering
{
    class ShaderBindingSetLayout;
    class DescriptorPool;

    class ShaderBindingSet
    {
    public:
        ShaderBindingSet() = default;
        virtual ~ShaderBindingSet() = default;

        virtual bool Initialize(const DescriptorPool& descriptorPool, const ShaderBindingSetLayout& bindingSetLayout) = 0;
        virtual void Destroy() = 0;
    };
}
