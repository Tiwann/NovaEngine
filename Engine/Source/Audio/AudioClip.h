#pragma once
#include "Runtime/Asset.h"
#include "Containers/StringView.h"
#include <miniaudio.h>


namespace Nova
{
    class AudioClip : public Asset
    {
    public:
        bool LoadFromFile(StringView filepath);
        bool LoadFromMemory(const void* data, size_t size);
        void Destroy();

        String GetAssetType() const override;

        ma_sound* GetHandle();
        const ma_sound* GetHandle() const;
    private:
        ma_sound m_Handle = { };
    };
}
