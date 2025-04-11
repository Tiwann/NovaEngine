#pragma once
#include "ResourceManager.h"
#include "Rendering/Texture2D.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(TextureManager, "TEXTURE MANAGER")

namespace Nova
{
    class TextureManager : public ResourceManager<Texture2D>
    {
    public:
        TextureManager() = default;

        Texture2D* Load(const String& Name, const Path& Filepath) override;
        Texture2D* Load(const String& Name, const Path& Filepath, const TextureParams& Params);
        Texture2D* Retrieve(const String& Name) override;
    };

    
}
