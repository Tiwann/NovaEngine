#include "Material.h"


namespace Nova
{
    Material::Material(Shader* InShader) : m_Shader(InShader)
    {

    }

    String Material::GetAssetType() const
    {
        return "Material";
    }
}
