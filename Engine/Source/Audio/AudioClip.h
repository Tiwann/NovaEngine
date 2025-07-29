#pragma once
#include "Game/Asset.h"
#include "Containers/StringView.h"
#include <miniaudio.h>


namespace Nova
{
    class AudioClip : public Asset
    {
    public:
        bool LoadFromFile(StringView filepath);
        bool LoadFromMemory(const void* data, size_t size);

        String GetAssetType() const override;

        ma_sound* GetHandle();
        const ma_sound* GetHandle() const;
    private:
        friend class AudioSystem;
        explicit AudioClip(AudioSystem* inSystem);
    private:
        AudioSystem* m_System = nullptr;
        ma_sound m_Handle;
    };
}
