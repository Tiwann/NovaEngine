#include "Font.h"
#include "Graphics/Texture2D.h"

namespace Nova
{
    Font::Font(const String& Name) : Asset(Name)
    {
        NOVA_ASSERT(false, "Fonts are not implemented yet!");
    }

    String Font::GetAssetType() const
    {
        return "Font";
    }

    bool Font::LoadFromFile(const Path& Filepath)
    {
        NOVA_ASSERT(false, "Fonts are not implemented yet!");
        return false;
    }
    
}
