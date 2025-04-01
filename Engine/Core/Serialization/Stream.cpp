#include "Stream.h"

bool Nova::Stream::IsOpened() const
{
    return m_Opened;
}

Nova::Stream::SizeType Nova::Stream::ReadLine(String& OutLine)
{
    if (!IsGood())
    {
        OutLine = "";
        return EndOfFile;
    }

    SizeType Count = 0;
    StaticArray<char, 1024> TempBuffer;
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
        OutLine = "";
        return IsGood() ? 0 : EndOfFile;
    }
    
    OutLine = String(TempBuffer.Data(), Count);
    OutLine.TrimEnd(' ');
    return IsGood() ? OutLine.Count() : EndOfFile;
    
}

void Nova::Stream::Close()
{
    m_Opened = false;
}

Nova::Stream::SizeType Nova::Stream::ReadChar(char& OutChar)
{
    return Read(&OutChar, sizeof(char));
}

Nova::Stream::SizeType Nova::Stream::ReadInt8(i8& OutInt)
{
    return Read(&OutInt, sizeof(i8));
}

Nova::Stream::SizeType Nova::Stream::ReadInt16(i16& OutInt)
{
    return Read(&OutInt, sizeof(i16));
}

Nova::Stream::SizeType Nova::Stream::ReadInt32(i32& OutInt)
{
    return Read(&OutInt, sizeof(i32));
}

Nova::Stream::SizeType Nova::Stream::ReadInt64(i64& OutInt)
{
    return Read(&OutInt, sizeof(i64));
}

Nova::Stream::SizeType Nova::Stream::ReadUInt8(u8& OutInt)
{
    return Read(&OutInt, sizeof(u8));
}

Nova::Stream::SizeType Nova::Stream::ReadUInt16(u16& OutInt)
{
    return Read(&OutInt, sizeof(u16));
}

Nova::Stream::SizeType Nova::Stream::ReadUInt32(u32& OutInt)
{
    return Read(&OutInt, sizeof(u32));
}

Nova::Stream::SizeType Nova::Stream::ReadUInt64(u64& OutInt)
{
    return Read(&OutInt, sizeof(u64));
}

Nova::Stream::SizeType Nova::Stream::ReadFloat(f32& OutFloat)
{
    return Read(&OutFloat, sizeof(f32));
}

Nova::Stream::SizeType Nova::Stream::ReadDouble(double& OutDouble)
{
    return Read(&OutDouble, sizeof(double));
}

Nova::Stream::SizeType Nova::Stream::ReadString(String& Str, SizeType Count)
{
    String::CharacterType* Characters = new String::CharacterType[Count];
    const Stream::SizeType ReadChars = Read(Characters, Count);
    Str = String(Characters, Count);
    delete[] Characters;
    return ReadChars;
}

Nova::Stream::SizeType Nova::Stream::WriteChar(const char InChar)
{
    return Write(&InChar, sizeof(char));
}

Nova::Stream::SizeType Nova::Stream::WriteInt8(const i8& InInt)
{
    return Write(&InInt, sizeof(i8));
}

Nova::Stream::SizeType Nova::Stream::WriteInt16(const i16& InInt)
{
    return Write(&InInt, sizeof(i16));
}

Nova::Stream::SizeType Nova::Stream::WriteInt32(const i32& InInt)
{
    return Write(&InInt, sizeof(i32));
}

Nova::Stream::SizeType Nova::Stream::WriteInt64(const i64& InInt)
{
    return Write(&InInt, sizeof(i64));
}

Nova::Stream::SizeType Nova::Stream::WriteUInt8(const u8& InInt)
{
    return Write(&InInt, sizeof(u8));
}

Nova::Stream::SizeType Nova::Stream::WriteUInt16(const u16& InInt)
{
    return Write(&InInt, sizeof(u16));
}

Nova::Stream::SizeType Nova::Stream::WriteUInt32(const u32& InInt)
{
    return Write(&InInt, sizeof(u32));
}

Nova::Stream::SizeType Nova::Stream::WriteUInt64(const u64& InInt)
{
    return Write(&InInt, sizeof(u64));
}

Nova::Stream::SizeType Nova::Stream::WriteString(const String& Str)
{
    return Write(*Str, Str.Count());
}

Nova::Stream::SizeType Nova::Stream::WriteString(StringView Str)
{
    return Write(Str.Data(), Str.Count());
}

Nova::Stream::SizeType Nova::Stream::WriteFloat(f32 Float)
{
    return Write(&Float, sizeof(f32));
}

Nova::Stream::SizeType Nova::Stream::WriteDouble(double Double)
{
    return Write(&Double, sizeof(double));
}
