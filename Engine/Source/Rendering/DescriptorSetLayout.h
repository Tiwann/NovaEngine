#pragma once
#include "DescriptorType.h"
#include "ShaderStage.h"
#include "Containers/Array.h"
#include "Interfaces/Equatable.h"

namespace Nova
{
    struct DescriptorSetBinding : IStructEquatable<DescriptorSetBinding>
    {
        u32 Index;
        DescriptorType Type;
        ShaderStage ShaderStage;
    };

    class DescriptorSetLayout
    {
    public:
        void AddBinding(const DescriptorSetBinding& Binding);
        void AddBinding(const Array<DescriptorSetBinding>& Bindings);
        void RemoveBinding(const DescriptorSetBinding& Binding);


        const Array<DescriptorSetBinding>& GetBindings() const;
    private:
        Array<DescriptorSetBinding> m_Bindings;
    };
}
