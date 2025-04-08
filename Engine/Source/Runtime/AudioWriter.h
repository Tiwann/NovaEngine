#pragma once
#include "NumericTypes.h"
#include "Serialization/Stream.h"

namespace Nova
{

    struct AudioFormat
    {
        u32 Channels;
        u32 SampleRate;
        enum class SampleFormat
        {
            Unknown,
            F32,
            S32,
            S16,
            S8,
            U8
        };
        SampleFormat SampleFormat;

        u32 GetBitsPerSample() const
        {
            switch (SampleFormat)
            {
            case SampleFormat::Unknown: return 0;
            case SampleFormat::F32: return 4;
            case SampleFormat::S32: return 4;
            case SampleFormat::S16: return 2;
            case SampleFormat::S8: return 1;
            case SampleFormat::U8: return 1;
            }
            return 0;
        }
        
    };

    
    class AudioWriter
    {
    public:
        AudioWriter(const AudioFormat& InFormat, const AudioFormat& OutFormat) : m_InFormat(InFormat), m_OutFormat(OutFormat) {}
        virtual ~AudioWriter() = default;
        virtual bool Write(BufferView<u8> Samples, Stream& OutStream) = 0;
    protected:
        AudioFormat m_InFormat;
        AudioFormat m_OutFormat;
    };

    class WavWriter : public AudioWriter
    {
    public:
        WavWriter(const AudioFormat& InFormat, const AudioFormat& OutFormat) : AudioWriter(InFormat, OutFormat) {}
        bool Write(BufferView<u8> Samples, Stream& OutStream) override;
    };

    class OggWriter : public AudioWriter
    {
    public:
        OggWriter(const AudioFormat& InFormat, const AudioFormat& OutFormat) : AudioWriter(InFormat, OutFormat) {}
        bool Write(BufferView<u8> Samples, Stream& OutStream) override;
    };
    
    class Mp3Writer : public AudioWriter
    {
    public:
        Mp3Writer(const AudioFormat& InFormat, const AudioFormat& OutFormat) : AudioWriter(InFormat, OutFormat) {}
        bool Write(BufferView<u8> Samples, Stream& OutStream) override;
    };
}
