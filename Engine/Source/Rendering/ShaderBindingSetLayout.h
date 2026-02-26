#pragma once
#include "BindingType.h"
#include "ShaderStage.h"
#include "Containers/Map.h"
#include "Containers/StringView.h"

#include <cstdint>

#include "Runtime/Object.h"

namespace Nova
{
    class RenderDevice;

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

    class ShaderBindingSetLayout : public Object, public Iterable<Pair<uint32_t, ShaderBinding>>
    {
    public:
        using BindingMap = Map<uint32_t, ShaderBinding>;

        ShaderBindingSetLayout() = default;
        ~ShaderBindingSetLayout() override = default;

        virtual bool Initialize(RenderDevice* device, uint32_t setIndex) = 0;
        virtual void Destroy() = 0;
        virtual bool Build() = 0;

        size_t BindingCount() const;
        void SetBinding(uint32_t index, const ShaderBinding& binding);
        const ShaderBinding& GetBinding(uint32_t index) const;

        const BindingMap& GetBindings() const { return m_Bindings; }

        uint32_t GetSetIndex() const { return m_SetIndex; }

        BindingMap::Iterator begin() override;
        BindingMap::Iterator end() override;
        BindingMap::ConstIterator begin() const override;
        BindingMap::ConstIterator end() const override;
    protected:
        BindingMap m_Bindings;
        uint32_t m_SetIndex = 0;
    };
}
