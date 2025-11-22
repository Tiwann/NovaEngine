#pragma once
#pragma warning(disable:4146)
#include "OpenMode.h"
#include "Seek.h"
#include "Runtime/Flags.h"
#include "Containers/StringView.h"

namespace Nova
{
    class Stream
    {
    public:
        using SizeType = size_t;
        using OffsetType = off_t;

        static constexpr SizeType EndOfFile = SizeType(~0);

        Stream() = default;
        explicit Stream(const OpenModeFlags openMode) : m_OpenMode(openMode) {}
        virtual ~Stream() { Close(); }

        bool IsOpened() const;

        virtual bool IsGood() const = 0;

        virtual SizeType ReadRaw(void* outBuffer, SizeType size) = 0;
        virtual SizeType WriteRaw(const void* inBuffer, SizeType size) = 0;
        virtual bool Seek(Seek seekMode, OffsetType offset) = 0;
        virtual OffsetType Tell() const = 0;
        virtual void Close();

        bool Rewind() { Seek(Seek::Begin, 0); return IsOpened(); }

        SizeType ReadLine(String& outLine);
        SizeType Read(String& str, SizeType count);
        SizeType Read(char& outChar);
        SizeType Read(int8_t& outInt);
        SizeType Read(int16_t& outInt);
        SizeType Read(int32_t& outInt);
        SizeType Read(int64_t& outInt);
        SizeType Read(uint8_t& outInt);
        SizeType Read(uint16_t& outInt);
        SizeType Read(uint32_t& outInt);
        SizeType Read(uint64_t& outInt);
        SizeType Read(float& outFloat);
        SizeType Read(double& outDouble);

        template<typename Type>
        SizeType ReadBuffer(BufferView<Type>& outBuffer)
        {
            return ReadRaw(outBuffer.Data(), outBuffer.Size());
        }

        template<typename T>
        SizeType ReadObject(T& obj)
        {
            return ReadRaw((void*)&obj, sizeof(T));
        }

        template<typename T>
        SizeType WriteObject(const T& obj)
        {
            return WriteRaw((void*)&obj, sizeof(T));
        }

        SizeType Write(char value);
        SizeType Write(int8_t value);
        SizeType Write(int16_t value);
        SizeType Write(int32_t value);
        SizeType Write(int64_t value);
        SizeType Write(uint8_t value);
        SizeType Write(uint16_t value);
        SizeType Write(uint32_t value);
        SizeType Write(uint64_t value);
        SizeType Write(const String& string);
        SizeType Write(StringView string);
        SizeType Write(float value);
        SizeType Write(double value);

    protected:
        bool m_Opened = false;
        OpenModeFlags m_OpenMode = OpenModeFlagBits::None;
    };
}

