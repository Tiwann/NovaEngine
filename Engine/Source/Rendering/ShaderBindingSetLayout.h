#pragma once
#include "ResourceBindingType.h"
#include "ShaderStage.h"
#include "Containers/Map.h"
#include "Containers/StringView.h"

#include <cstdint>

namespace Nova::Rendering
{
    class Device;

    struct ShaderBindingSetLayoutCreateInfo
    {
        Device* device = nullptr;
    };

    struct ShaderBinding
    {
        StringView name;
        ShaderStageFlags stageFlags;
        ResourceBindingType bindingType;
    };

    class ShaderBindingSetLayout
    {
        using BindingMap = Map<uint32_t, ShaderBinding>;
    public:
        ShaderBindingSetLayout() = default;
        virtual ~ShaderBindingSetLayout() = default;

        ShaderBindingSetLayout(const ShaderBindingSetLayout&) = delete;
        ShaderBindingSetLayout& operator=(const ShaderBindingSetLayout&) = delete;

        virtual bool Initialize(const ShaderBindingSetLayoutCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        virtual bool Build() = 0;

        void SetBinding(uint32_t index, const ShaderBinding& binding);
        const ShaderBinding& GetBinding(uint32_t index) const;

        BindingMap::Iterator begin();
        BindingMap::Iterator end();
        BindingMap::ConstIterator begin() const;
        BindingMap::ConstIterator end() const;
    protected:
        BindingMap m_Bindings;
    };
}
