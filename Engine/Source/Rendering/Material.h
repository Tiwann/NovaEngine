#pragma once
#include "Runtime/Asset.h"

namespace Nova
{
    enum class MaterialShadingModel
    {
        None,
        DefaultLit,
        Unlit,
    };

    enum class MaterialBlendingMode
    {
        None,
        Opaque,
        Transparent,
        Cutout
    };

    struct MaterialInfo
    {
        MaterialShadingModel ShadingModel;
        MaterialBlendingMode BlendingMode;
    };

    class Material : public Asset
    {
    public:
        explicit Material(class Shader* InShader);
        String GetAssetType() const override;


    private:
        class Shader* m_Shader = nullptr;
        MaterialShadingModel m_ShadingModel = MaterialShadingModel::None;
        MaterialBlendingMode m_BlendingMode = MaterialBlendingMode::None;
    };
}
