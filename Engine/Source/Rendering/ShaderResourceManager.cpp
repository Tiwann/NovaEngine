#include "ShaderResourceManager.h"

namespace Nova::Rendering
{
    void ShaderResourceManager::AddBindingSetLayout(const ShaderBindingSetLayout& setLayout)
    {
        m_SetLayouts.Add(&setLayout);
    }

    const ShaderBindingSetLayout& ShaderResourceManager::GetBindingSetLayout(size_t index)
    {
        return *m_SetLayouts[index];
    }
}
