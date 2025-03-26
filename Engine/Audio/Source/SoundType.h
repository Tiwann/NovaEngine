#pragma once
#include "Core/Containers/String.h"

namespace Hydro
{
    enum class SoundType
    {
        Unknown,
        AIFF,
        ASF,
        DLS,
        FLAC,
        FSB,
        IT,
        MIDI,
        MOD,
        MPEG,
        OGGVORBIS,
        PLAYLIST,
        RAW,
        S3M,
        USER,
        WAV,
        XM,
        XMA,
        AUDIOQUEUE,
        AT9,
        VORBIS,
        MEDIAFOUNDATION,
        MEDIACODEC,
        FADPCM,
        OPUS,
    };

    inline String SoundTypeToString(SoundType Type)
    {
        switch(Type)
        {
        case SoundType::Unknown: return "Unknown";
        case SoundType::AIFF: return "AIFF";
        case SoundType::ASF: return "ASF";
        case SoundType::DLS: return "DLS";
        case SoundType::FLAC: return "FLAC";
        case SoundType::FSB: return "FSB";
        case SoundType::IT: return "IT";
        case SoundType::MIDI: return "MIDI";
        case SoundType::MOD: return "MOD";
        case SoundType::MPEG: return "MPEG";
        case SoundType::OGGVORBIS: return "OGG VORBIS";
        case SoundType::PLAYLIST: return "PLAYLIST";
        case SoundType::RAW: return "RAW";
        case SoundType::S3M: return "S3M";
        case SoundType::USER: return "USER";
        case SoundType::WAV: return "WAV";
        case SoundType::XM: return "XM";
        case SoundType::XMA: return "XMA";
        case SoundType::AUDIOQUEUE: return "AUDIO QUEUE";
        case SoundType::AT9: return "AT9";
        case SoundType::VORBIS: return "VORBIS";
        case SoundType::MEDIAFOUNDATION: return "MEDIA FOUNDATION";
        case SoundType::MEDIACODEC: return "MEDIA CODEC";
        case SoundType::FADPCM: return "FADPCM";
        case SoundType::OPUS: return "OPUS";
        }
        return "Unknown";
    }
}
