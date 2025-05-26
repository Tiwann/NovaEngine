#pragma once
#include "Runtime/Asset.h"

namespace Nova
{
    class Shader;

    class PostProcessEffect : public Asset
    {
    public:
        explicit PostProcessEffect(const String& Name) : Asset(Name) {}

        String GetAssetType() const override;

        void SetShader(Shader* Shader);
    private:
        Shader* m_Shader = nullptr;
    };
}
