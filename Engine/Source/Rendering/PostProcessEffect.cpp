#include "PostProcessEffect.h"

namespace Nova
{
    String PostProcessEffect::GetAssetType() const
    {
        return "PostProcessEffect";
    }

    void PostProcessEffect::SetShader(Shader* Shader)
    {
        m_Shader = Shader;
    }

}
