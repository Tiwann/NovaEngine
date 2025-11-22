#include "Stream.h"

namespace Nova
{
    bool Stream::IsOpened() const
    {
        return m_Opened;
    }

    Stream::SizeType Stream::ReadLine(String& outLine)
    {
        if (!IsGood()) return IsGood() ? 0 : EndOfFile;

        SizeType count = 0;
        char buffer[1024];

        while (IsGood())
        {
            char character;
            Read(character);
            if (character == '\n')
                break;

            if (character == '\r')
            {
                const OffsetType position = Tell();
                Read(character);
                if (IsGood() && character == '\n')
                    break;

                Seek(Seek::Begin, position);
                break;
            }

            buffer[count] = character;
            count++;
        }

        if (count == 0) return IsGood() ? 0 : EndOfFile;

        outLine = std::move(buffer);
        return IsGood() ? outLine.Count() : EndOfFile;
    }

    void Stream::Close()
    {
        m_Opened = false;
    }

    Stream::SizeType Stream::Read(char& outChar)
    {
        return ReadRaw(&outChar, sizeof(char));
    }

    Stream::SizeType Stream::Read(int8_t& outInt)
    {
        return ReadRaw(&outInt, sizeof(int8_t));
    }

    Stream::SizeType Stream::Read(int16_t& outInt)
    {
        return ReadRaw(&outInt, sizeof(int16_t));
    }

    Stream::SizeType Stream::Read(int32_t& outInt)
    {
        return ReadRaw(&outInt, sizeof(int32_t));
    }

    Stream::SizeType Stream::Read(int64_t& outInt)
    {
        return ReadRaw(&outInt, sizeof(int64_t));
    }

    Stream::SizeType Stream::Read(uint8_t& outInt)
    {
        return ReadRaw(&outInt, sizeof(uint8_t));
    }

    Stream::SizeType Stream::Read(uint16_t& outInt)
    {
        return ReadRaw(&outInt, sizeof(uint16_t));
    }

    Stream::SizeType Stream::Read(uint32_t& outInt)
    {
        return ReadRaw(&outInt, sizeof(uint32_t));
    }

    Stream::SizeType Stream::Read(uint64_t& outInt)
    {
        return ReadRaw(&outInt, sizeof(uint64_t));
    }

    Stream::SizeType Stream::Read(float& outFloat)
    {
        return ReadRaw(&outFloat, sizeof(float));
    }

    Stream::SizeType Stream::Read(double& outDouble)
    {
        return ReadRaw(&outDouble, sizeof(double));
    }

    Stream::SizeType Stream::Read(String& str, const SizeType count)
    {
        const SizeType bytesToRead = Math::Max(count, str.Count());
        const SizeType bytesRead = ReadRaw(*str, bytesToRead);
        return bytesRead;
    }

    Stream::SizeType Stream::Write(const char value)
    {
        return WriteRaw(&value, sizeof(char));
    }

    Stream::SizeType Stream::Write(int8_t value)
    {
        return WriteRaw(&value, sizeof(int8_t));
    }

    Stream::SizeType Stream::Write(int16_t value)
    {
        return WriteRaw(&value, sizeof(int16_t));
    }

    Stream::SizeType Stream::Write(int32_t value)
    {
        return WriteRaw(&value, sizeof(int32_t));
    }

    Stream::SizeType Stream::Write(int64_t value)
    {
        return WriteRaw(&value, sizeof(int64_t));
    }

    Stream::SizeType Stream::Write(uint8_t value)
    {
        return WriteRaw(&value, sizeof(uint8_t));
    }

    Stream::SizeType Stream::Write(uint16_t value)
    {
        return WriteRaw(&value, sizeof(uint16_t));
    }

    Stream::SizeType Stream::Write(uint32_t value)
    {
        return WriteRaw(&value, sizeof(uint32_t));
    }

    Stream::SizeType Stream::Write(uint64_t value)
    {
        return WriteRaw(&value, sizeof(uint64_t));
    }

    Stream::SizeType Stream::Write(const String& string)
    {
        return WriteRaw(*string, string.Size());
    }

    Stream::SizeType Stream::Write(const StringView string)
    {
        return WriteRaw(*string, string.Size());
    }

    Stream::SizeType Stream::Write(const float value)
    {
        return WriteRaw(&value, sizeof(float));
    }

    Stream::SizeType Stream::Write(const double value)
    {
        return WriteRaw(&value, sizeof(double));
    }
}
