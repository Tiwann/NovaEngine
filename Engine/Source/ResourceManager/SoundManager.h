#pragma once
#include "ResourceManager.h"
#include "Audio/Sound.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(SoundManager, "SOUND MANAGER")

namespace Nova
{
    class SoundManager : public ResourceManager<Sound>
    {
    public:
        Sound* Load(const String& Name, const Path& Filepath, SoundFlags Flags);
        Sound* Load(const String& Name, const Path& Filepath) override;
        Sound* Retrieve(const String& Name) override;
    };
}
