#pragma once
#include "Containers/Array.h"

namespace Nova::Rendering
{
    class Device;
    class ShaderBindingSetLayout;

    struct ShaderResourceManagerCreateInfo
    {
        Device* device = nullptr;
    };

    class ShaderResourceManager
    {
    public:
        ShaderResourceManager() = default;
        virtual ~ShaderResourceManager() = default;

        virtual bool Initialize(const ShaderResourceManagerCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        void AddBindingSetLayout(const ShaderBindingSetLayout& setLayout);
        const ShaderBindingSetLayout& GetBindingSetLayout(size_t index);
    protected:
        Array<const ShaderBindingSetLayout*> m_SetLayouts;
    };
};
