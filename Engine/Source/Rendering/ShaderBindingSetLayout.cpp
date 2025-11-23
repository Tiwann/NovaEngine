#include "ShaderBindingSetLayout.h"

namespace Nova
{
    size_t ShaderBindingSetLayout::BindingCount() const
    {
        return m_Bindings.Count();
    }

    void ShaderBindingSetLayout::SetBinding(const uint32_t index, const ShaderBinding& binding)
    {
        m_Bindings[index] = binding;
    }

    const ShaderBinding& ShaderBindingSetLayout::GetBinding(const uint32_t index) const
    {
        return m_Bindings[index];
    }

    ShaderBindingSetLayout::BindingMap::Iterator ShaderBindingSetLayout::begin()
    {
        return m_Bindings.begin();
    }

    ShaderBindingSetLayout::BindingMap::Iterator ShaderBindingSetLayout::end()
    {
        return m_Bindings.end();
    }

    ShaderBindingSetLayout::BindingMap::ConstIterator ShaderBindingSetLayout::begin() const
    {
        return m_Bindings.begin();
    }

    ShaderBindingSetLayout::BindingMap::ConstIterator ShaderBindingSetLayout::end() const
    {
        return m_Bindings.end();
    }
}
