#include "Stream.h"

namespace Nova
{
    bool Stream::IsOpened() const
    {
        return m_Opened;
    }

    Stream::SizeType Stream::ReadLine(String& outLine)
    {
        if (!IsGood())
        {
            outLine = "";
            return EndOfFile;
        }

        SizeType Count = 0;
        char TempBuffer[1024];
        while (IsGood())
        {
            char Character;
            ReadChar(Character);
            if (Character == '\n')
                break;

            if (Character == '\r')
            {
                const OffsetType Position = Tell();
                ReadChar(Character);
                if (IsGood() && Character == '\n')
                    break;

                Seek(Seek::Begin, Position);
                break;
            }

            TempBuffer[Count] = Character;
            Count++;
        }

        if (Count == 0)
        {
            outLine = "";
            return IsGood() ? 0 : EndOfFile;
        }

        outLine = String(TempBuffer, Count);
        outLine.TrimEnd(' ');
        return IsGood() ? outLine.Count() : EndOfFile;
    }

    void Stream::Close()
    {
        m_Opened = false;
    }

    Stream::SizeType Stream::ReadChar(char& outChar)
    {
        return Read(&outChar, sizeof(char));
    }

    Stream::SizeType Stream::ReadInt8(int8_t& outInt)
    {
        return Read(&outInt, sizeof(int8_t));
    }

    Stream::SizeType Stream::ReadInt16(int16_t& outInt)
    {
        return Read(&outInt, sizeof(int16_t));
    }

    Stream::SizeType Stream::ReadInt32(int32_t& outInt)
    {
        return Read(&outInt, sizeof(int32_t));
    }

    Stream::SizeType Stream::ReadInt64(int64_t& outInt)
    {
        return Read(&outInt, sizeof(int64_t));
    }

    Stream::SizeType Stream::ReadUInt8(uint8_t& outInt)
    {
        return Read(&outInt, sizeof(uint8_t));
    }

    Stream::SizeType Stream::ReadUInt16(uint16_t& outInt)
    {
        return Read(&outInt, sizeof(uint16_t));
    }

    Stream::SizeType Stream::ReadUInt32(uint32_t& outInt)
    {
        return Read(&outInt, sizeof(uint32_t));
    }

    Stream::SizeType Stream::ReadUInt64(uint64_t& outInt)
    {
        return Read(&outInt, sizeof(uint64_t));
    }

    Stream::SizeType Stream::ReadFloat(float& outFloat)
    {
        return Read(&outFloat, sizeof(float));
    }

    Stream::SizeType Stream::ReadDouble(double& outDouble)
    {
        return Read(&outDouble, sizeof(double));
    }

    Stream::SizeType Stream::ReadString(String& str, const SizeType count)
    {
        String::CharacterType* characters = new String::CharacterType[count];
        const SizeType readChars = Read(characters, count);
        str = String(characters, count);
        delete[] characters;
        return readChars;
    }

    Stream::SizeType Stream::WriteChar(const char value)
    {
        return Write(&value, sizeof(char));
    }

    Stream::SizeType Stream::WriteInt8(const int8_t& value)
    {
        return Write(&value, sizeof(int8_t));
    }

    Stream::SizeType Stream::WriteInt16(const int16_t& value)
    {
        return Write(&value, sizeof(int16_t));
    }

    Stream::SizeType Stream::WriteInt32(const int32_t& value)
    {
        return Write(&value, sizeof(int32_t));
    }

    Stream::SizeType Stream::WriteInt64(const int64_t& value)
    {
        return Write(&value, sizeof(int64_t));
    }

    Stream::SizeType Stream::WriteUInt8(const uint8_t& value)
    {
        return Write(&value, sizeof(uint8_t));
    }

    Stream::SizeType Stream::WriteUInt16(const uint16_t& value)
    {
        return Write(&value, sizeof(uint16_t));
    }

    Stream::SizeType Stream::WriteUInt32(const uint32_t& value)
    {
        return Write(&value, sizeof(uint32_t));
    }

    Stream::SizeType Stream::WriteUInt64(const uint64_t& value)
    {
        return Write(&value, sizeof(uint64_t));
    }

    Stream::SizeType Stream::WriteString(const String& str)
    {
        return Write(*str, str.Count());
    }

    Stream::SizeType Stream::WriteString(const StringView str)
    {
        return Write(str.Data(), str.Count());
    }

    Stream::SizeType Stream::WriteFloat(const float value)
    {
        return Write(&value, sizeof(float));
    }

    Stream::SizeType Stream::WriteDouble(const double value)
    {
        return Write(&value, sizeof(double));
    }
}
