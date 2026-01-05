#pragma once
#include "AssetLoader.h"

namespace Nova
{
    class TextureLoader : public AssetLoader
    {
    public:
        AssetType GetAssetType() const final { return AssetType::Texture; }
    private:

    };
}
