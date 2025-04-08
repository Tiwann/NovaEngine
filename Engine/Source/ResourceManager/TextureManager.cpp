#include "TextureManager.h"

#include "Core/Log.h"
#include "Core/LogVerbosity.h"
#include "Core/Graphics/Texture2D.h"
#include "Core/Containers/StringFormat.h"

namespace Nova
{
    Texture2D* TextureManager::Load(const String& Name, const Path& Filepath)
    {
        constexpr TextureParams DefaultParams = {
            .Filter = TextureFilter::Nearest,
            .Wrap = TextureWrap::Repeat,
        };
        
        Texture2D* LoadedTexture = Texture2D::CreateFromFile(*Name, Filepath, DefaultParams);
        if(!LoadedTexture) return nullptr;
        m_Data[Name] = LoadedTexture;
        return LoadedTexture;
    }

    Texture2D* TextureManager::Load(const String& Name, const Path& Filepath, const TextureParams& Params)
    {
        Texture2D* LoadedTexture = Texture2D::CreateFromFile(*Name, Filepath, Params);
        if(!LoadedTexture) return nullptr;
        m_Data[Name] = LoadedTexture;
        return LoadedTexture;
    }

    Texture2D* TextureManager::Retrieve(const String& Name)
    {
        if(!m_Data.Contains(Name))
        {
            NOVA_LOG(TextureManager, Verbosity::Warning, "Texture \"{}\" not found!", Name);
            return nullptr;
        }
        return m_Data[Name];
    }
}
