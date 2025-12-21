#include "Path.h"

#include "Containers/StringFormat.h"

namespace Nova
{
    String Path::Combine(const StringView path, const StringView other)
    {
        return StringFormat("{}{}{}", path, s_Separator, other).ReplaceAll(s_OtherSeparator, s_Separator);
    }

    StringView Path::GetEngineDirectory()
    {
        return NOVA_ENGINE_ROOT_DIR;
    }

    String Path::GetEngineAssetsDirectory()
    {
        return Combine(GetEngineDirectory(), "Assets");
    }

    String Path::GetEngineAssetPath(const StringView filepath)
    {
        return Combine(GetEngineAssetsDirectory(), filepath);
    }
}
