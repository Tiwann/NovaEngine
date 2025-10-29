#pragma once
#include "BindingType.h"
#include "ShaderStage.h"
#include "Containers/Map.h"
#include "Containers/StringView.h"

#include <cstdint>

namespace Nova::Rendering
{
    class Device;

    struct ShaderBinding
    {
        StringView name;
        ShaderStageFlags stageFlags = ShaderStageFlagBits::None;
        BindingType bindingType;
        uint32_t arrayCount = 1;

        bool operator==(const ShaderBinding& other) const
        {
            return name == other.name &&
            stageFlags == other.stageFlags &&
            bindingType == other.bindingType &&
            arrayCount == other.arrayCount;
        }
    };

    class ShaderBindingSetLayout
    {
        using BindingMap = Map<uint32_t, ShaderBinding>;
    public:
        ShaderBindingSetLayout() = default;
        virtual ~ShaderBindingSetLayout() = default;

        virtual bool Initialize(Device* device) = 0;
        virtual void Destroy() = 0;
        virtual bool Build() = 0;

        size_t BindingCount() const;
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
