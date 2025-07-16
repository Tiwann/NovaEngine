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

        static constexpr SizeType EndOfFile = -1;

        Stream() = default;
        explicit Stream(const OpenModeFlags openMode) : m_OpenMode(openMode) {}
        virtual ~Stream() { Close(); }

        bool IsOpened() const;

        virtual bool IsGood() const = 0;

        virtual SizeType Read(void* outBuffer, SizeType size) = 0;
        virtual SizeType Write(const void* inBuffer, SizeType size) = 0;
        virtual bool Seek(Seek seekMode, OffsetType offset) = 0;
        virtual OffsetType Tell() const = 0;
        virtual void Close();

        SizeType ReadLine(String& outLine);
        SizeType ReadChar(char& outChar);
        SizeType ReadInt8(int8_t& outInt);
        SizeType ReadInt16(int16_t& outInt);
        SizeType ReadInt32(int32_t& outInt);
        SizeType ReadInt64(int64_t& outInt);
        SizeType ReadUInt8(uint8_t& outInt);
        SizeType ReadUInt16(uint16_t& outInt);
        SizeType ReadUInt32(uint32_t& outInt);
        SizeType ReadUInt64(uint64_t& outInt);
        SizeType ReadFloat(float& outFloat);
        SizeType ReadDouble(double& outDouble);

        template<typename Type>
        SizeType ReadBuffer(Buffer<Type>& outBuffer, SizeType count)
        {
            outBuffer = Buffer<Type>(count);
            return Read(outBuffer.Data(), count * sizeof(Type));
        }

        template<typename Struct>
        SizeType ReadStruct(Struct* where, SizeType count)
        {
            return Read(where, count * sizeof(Struct));
        }

        SizeType ReadString(String& str, SizeType count);

        SizeType WriteChar(char value);
        SizeType WriteInt8(const int8_t& value);
        SizeType WriteInt16(const int16_t& value);
        SizeType WriteInt32(const int32_t& value);
        SizeType WriteInt64(const int64_t& value);
        SizeType WriteUInt8(const uint8_t& value);
        SizeType WriteUInt16(const uint16_t& value);
        SizeType WriteUInt32(const uint32_t& value);
        SizeType WriteUInt64(const uint64_t& value);
        SizeType WriteString(const String& str);
        SizeType WriteString(StringView str);
        SizeType WriteFloat(float value);
        SizeType WriteDouble(double value);

    protected:
        bool m_Opened = false;
        OpenModeFlags m_OpenMode = OpenModeFlagBits::None;
    };
}

