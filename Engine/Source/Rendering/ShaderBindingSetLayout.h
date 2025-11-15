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
        String name;
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

    class ShaderBindingSetLayout : public Iterable<Map<uint32_t, ShaderBinding>::PairType>
    {
    public:
        using BindingMap = Map<uint32_t, ShaderBinding>;

        ShaderBindingSetLayout() = default;
        ~ShaderBindingSetLayout() override = default;

        virtual bool Initialize(Device* device) = 0;
        virtual void Destroy() = 0;
        virtual bool Build() = 0;

        size_t BindingCount() const;
        void SetBinding(uint32_t index, const ShaderBinding& binding);
        const ShaderBinding& GetBinding(uint32_t index) const;

        const BindingMap& GetBindings() const { return m_Bindings; }

        BindingMap::Iterator begin() override;
        BindingMap::Iterator end() override;
        BindingMap::ConstIterator begin() const override;
        BindingMap::ConstIterator end() const override;
    protected:
        BindingMap m_Bindings;
    };
}
