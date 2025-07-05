#include "DescriptorSetLayout.h"

namespace Nova
{
    void DescriptorSetLayout::AddBinding(const DescriptorSetBinding& Binding)
    {
        m_Bindings.Add(Binding);
    }

    void DescriptorSetLayout::AddBinding(const Array<DescriptorSetBinding>& Bindings)
    {
        m_Bindings.AddRange(Bindings);
    }

    void DescriptorSetLayout::RemoveBinding(const DescriptorSetBinding& Binding)
    {
        m_Bindings.Remove(Binding);
    }

    const Array<DescriptorSetBinding>& DescriptorSetLayout::GetBindings() const
    {
        return m_Bindings;
    }
}
