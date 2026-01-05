#pragma once
#include "IO/Stream.h"
#include "Containers/StringView.h"
#include "Runtime/Asset.h"
#include "Runtime/AssetType.h"
#include "Runtime/Ref.h"

namespace Nova
{
    class AssetLoader
    {
    public:
        AssetLoader() = default;
        virtual ~AssetLoader() = default;

        virtual AssetType GetAssetType() const = 0;
        Ref<Asset> LoadFromFile(StringView filepath);
        virtual Ref<Asset> LoadFromStream(Stream& stream) = 0;
        virtual Array<String> SupportedExtensions() const = 0;
    protected:
        bool FilepathIsValid(StringView filepath) const;
    };
}
