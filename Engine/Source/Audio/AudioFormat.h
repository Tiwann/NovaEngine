#pragma once
#include "Runtime/TypeTraits.h"
#include "Runtime/Memory.h"
#include <cassert>
#include <cstdint>

namespace Nova
{
    typedef struct int24_t
    {
        uint8_t b0{0}, b1{0}, b2{0};
    } int24_t;
    static_assert(sizeof(int24_t) == 3);

    enum class SampleInterleaving
    {
        Interleaved,
        Contiguous
    };

    enum class SampleFormat
    {
        Float,
        Integer
    };

    struct AudioFormat
    {
        uint32_t channels;
        uint32_t sampleRate;
        uint32_t bytesPerSample;
        SampleInterleaving interleaving = SampleInterleaving::Interleaved;
        SampleFormat sampleFormat = SampleFormat::Float;
    };

    enum class AudioChannels
    {
        None = 0,
        Mono = 1,
        Stereo = 2,
        Surround_5_1 = 5,
        Surround_7_1 = 7,
    };

    template <typename T>
    struct IsAudioSample : FalseType {};

    template <>
    struct IsAudioSample<float> : TrueType {};

    template <>
    struct IsAudioSample<double> : TrueType {};

    template <>
    struct IsAudioSample<int32_t> : TrueType {};

    template <>
    struct IsAudioSample<int16_t> : TrueType {};

    template <>
    struct IsAudioSample<int24_t> : TrueType {};

    template <>
    struct IsAudioSample<uint8_t> : TrueType {};

    template <typename T>
    static constexpr bool IsSampleValue = IsAudioSample<T>::value;

    template <typename T>
    concept AudioSample = IsSampleValue<T>;

    template<typename T, AudioChannels> 
    struct AudioFrame;

    template<AudioSample T>
    struct AudioFrame<T, AudioChannels::Mono>
    {
        T center;
    };

    template<AudioSample T>
    struct AudioFrame<T, AudioChannels::Stereo>
    {
        T left, right;
    };

    template <AudioSample T>
    struct AudioFrame<T, AudioChannels::Surround_5_1>
    {
        T left, right, subwoofer, leftSurround, rightSurround;
    };

    template <AudioSample T>
    struct AudioFrame<T, AudioChannels::Surround_7_1>
    {
        T left, right, subwoofer, leftSurround, rightSurround, leftBackSurround, rightBackSurround;
    };


    template<AudioSample T, AudioChannels Channels = AudioChannels::Stereo>
    class AudioBuffer
    {
    public:

        explicit AudioBuffer(const AudioFormat& format, T* data, size_t size) : m_Format(format), m_Size(size)
        {
            assert(format.channels == (uint32_t)Channels);
            m_Data = new uint8_t[size]{};
            Memory::Memcpy(m_Data, data, size);
        }

        explicit AudioBuffer(const AudioFormat& format) : m_Format(format){}

        explicit AudioBuffer(const AudioFormat& format, size_t size) : m_Format(format)
        {
            assert(format.channels == (uint32_t)Channels);
            m_Data = new uint8_t[size]{};
        }

        explicit AudioBuffer(const AudioBuffer& other) : m_Format(other.m_Format)
        {
            if (m_Data && m_Size)
            {
                delete[] m_Data;
                m_Data = nullptr;
            }

            m_Data = new uint8_t[other.m_Size]{};
            Memory::Memcpy(m_Data, other.m_Data, other.m_Size);
        }

        explicit AudioBuffer(AudioBuffer&& other) noexcept : m_Format(other.m_Format)
        {
            if (m_Data && m_Size)
            {
                delete[] m_Data;
                m_Data = nullptr;
            }

            m_Data = other.m_Data;
            m_Size = other.m_Size;
            other.m_Data = nullptr;
            other.m_Size = 0;
        }

        AudioBuffer& operator=(const AudioBuffer& other)
        {
            if (this == &other)
                return *this;

            if (m_Data && m_Size)
            {
                delete[] m_Data;
                m_Data = nullptr;
            }

            m_Data = new uint8_t[other.m_Size]{};
            Memory::Memcpy(m_Data, other.m_Data, other.m_Size);
            return *this;
        }

        AudioBuffer& operator=(AudioBuffer&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (m_Data && m_Size)
            {
                delete[] m_Data;
                m_Data = nullptr;
            }

            m_Data = other.m_Data;
            m_Size = other.m_Size;
            other.m_Data = nullptr;
            other.m_Size = 0;
            return *this;
        }

        AudioFrame<T, Channels> GetFrame(size_t index)
        {
            AudioFrame<T, Channels> frame;
            if constexpr (Channels == AudioChannels::Mono)
            {
                frame.center = m_Data[index];
                return frame;
            }
            else if constexpr (Channels == AudioChannels::Stereo)
            {
                if (m_Format.interleaving == SampleInterleaving::Interleaved)
                {
                    frame.left = m_Data[index * sizeof(T)];
                    frame.right = m_Data[index * sizeof(T) + 1];
                    return frame;
                }
            } else
            {
                static_assert(false, "Not implemented yet.");
                return {};
            }

            return {};
        }

        T* GetWritePointer() { return reinterpret_cast<T*>(m_Data); }
        const T* GetReadPointer() const { return reinterpret_cast<T*>(m_Data); }
        AudioFormat GetFormat() const { return m_Format; }
    private:
        AudioFormat m_Format;
        uint8_t* m_Data = nullptr;
        size_t m_Size = 0;
    };
}
