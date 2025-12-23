#include "AssetPack.h"
#include "Audio/AudioClip.h"
#include "IO/FileStream.h"
#include "Runtime/Application.h"
#include "Runtime/FileUtils.h"
#include "Runtime/Version.h"
#include "Utils/TextureUtils.h"

namespace Nova
{
    struct AssetPackHeader
    {
        uint32_t magic;
        Version version;
        uint64_t fileSize;
        uint64_t assetCount;
    };

    static void ReadString(Stream& stream, String& string)
    {
        uint32_t length = 0;
        stream.Read(length);
        string.Resize(length);
        stream.Read(string, length);
    }

    bool AssetEntry::Read(Stream& stream)
    {
        ReadString(stream, virtualPath);
        stream.ReadRaw(&uuid, sizeof(UUID));
        stream.ReadRaw(&assetType, sizeof(AssetType));
        stream.ReadRaw(&chunkOffset, 2 * sizeof(uint64_t));
        return true;
    }

    bool AssetPack::ReadFromFile(StringView filepath)
    {
        FileStream stream(filepath, OpenModeFlagBits::ReadBinary);
        if (!stream.IsOpened()) return false;

        AssetPackHeader header;
        if (stream.ReadRaw(&header, sizeof(AssetPackHeader)) != sizeof(AssetPackHeader)) return false;

        if (!Memory::Memcmp("NPAK", &header.magic, 4)) return false;

        for (uint32_t i = 0; i < header.assetCount; i++)
        {
            AssetEntry entry;
            entry.Read(stream);
            m_AssetTable.Add(entry);
        }

        m_AssetBlob = FileUtils::ReadTillEnd(stream);
        return true;
    }

    Ref<Asset> AssetPack::LoadAssetFromVirtualPath(StringView virtualPath)
    {
        const auto predicate = [&virtualPath](const AssetEntry& entry) -> bool { return virtualPath == entry.virtualPath; };
        const AssetEntry* entry = m_AssetTable.Single(predicate);
        if (!entry) return nullptr;
        return LoadAssetFromEntry(*entry);
    }

    Ref<Asset> AssetPack::LoadAssetFromUUID(UUID uuid)
    {
        const auto predicate = [&uuid](const AssetEntry& entry) -> bool { return uuid == entry.uuid; };
        const AssetEntry* entry = m_AssetTable.Single(predicate);
        if (!entry) return nullptr;
        return LoadAssetFromEntry(*entry);
    }

    Ref<Asset> AssetPack::LoadAssetFromEntry(const AssetEntry& entry)
    {
        Application& application = Application::GetCurrentApplication();
        const uint8_t* assetData = &m_AssetBlob[entry.chunkOffset];
        const uint64_t assetDataSize = entry.chunkSize;

        switch (entry.assetType)
        {
        case AssetType::Texture:
            return LoadTexture(application.GetDevice(), assetData, assetDataSize);
        case AssetType::AudioClip:
            {
                Ref<AudioClip> loadedClip = new AudioClip();
                if (!loadedClip->LoadFromMemory(assetData, assetDataSize, AudioPlaybackFlagBits::None))
                    return nullptr;
                return loadedClip;
            }
        case AssetType::Shader:
            break;
        case AssetType::StaticMesh:
            break;
        default: return nullptr;
        }
        return nullptr;
    }


}
