#include "Font.h"
#include "Rendering/Texture2D.h"

namespace Nova
{
    Font::Font(const String& Name) : Asset(Name)
    {
        Assert(false, "Fonts are not implemented yet!");
    }

    String Font::GetAssetType() const
    {
        return "Font";
    }

    bool Font::LoadFromFile(const Path& Filepath)
    {
        Assert(false, "Fonts are not implemented yet!");
        return false;
    }
    
}
