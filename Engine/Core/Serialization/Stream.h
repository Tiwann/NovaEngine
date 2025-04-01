#pragma once
#pragma warning(disable:4146)
#include "OpenMode.h"
#include "Seek.h"
#include "Runtime/Filesystem.h"
#include "Runtime/Flags.h"
#include "Containers/StringView.h"

namespace Nova
{
    class Stream
    {
    public:
        using SizeType = size_t;
        using OffsetType = off_t;
        static constexpr SizeType EndOfFile = -1ULL;
        
        Stream(const OpenModeFlags OpenMode) : m_OpenMode(OpenMode){}
        virtual ~Stream()
        {
            Close();
        }
        
        bool IsOpened() const;

        virtual bool IsGood() const = 0;

        virtual SizeType Read(void* OutBuffer, SizeType Count) = 0;
        virtual SizeType Write(const void* InBuffer, SizeType Count) = 0;
        virtual bool Seek(Seek SeekMode, OffsetType Offset) = 0;
        virtual OffsetType Tell() const = 0;
        virtual void Close();

        SizeType ReadLine(String& OutLine);
        SizeType ReadChar(char& OutChar);
        SizeType ReadInt8(i8& OutInt);
        SizeType ReadInt16(i16& OutInt);
        SizeType ReadInt32(i32& OutInt);
        SizeType ReadInt64(i64& OutInt);
        SizeType ReadUInt8(u8& OutInt);
        SizeType ReadUInt16(u16& OutInt);
        SizeType ReadUInt32(u32& OutInt);
        SizeType ReadUInt64(u64& OutInt);
        SizeType ReadFloat(f32& OutFloat);
        SizeType ReadDouble(double& OutDouble);

        template<typename Type>
        SizeType ReadBuffer(Buffer<Type>& OutBuffer, SizeType Count)
        {
            OutBuffer = Buffer<Type>(Count);
            return Read(OutBuffer.Data(), Count);
        }

        template<typename Struct>
        SizeType ReadStruct(Struct* Where, SizeType Count)
        {
            return Read(Where, Count * sizeof(Struct));
        }

        
        SizeType ReadString(String& Str, SizeType Count);

        SizeType WriteChar(char InChar);
        SizeType WriteInt8(const i8& InInt);
        SizeType WriteInt16(const i16& InInt);
        SizeType WriteInt32(const i32& InInt);
        SizeType WriteInt64(const i64& InInt);
        SizeType WriteUInt8(const u8& InInt);
        SizeType WriteUInt16(const u16& InInt);
        SizeType WriteUInt32(const u32& InInt);
        SizeType WriteUInt64(const u64& InInt);
        SizeType WriteString(const String& Str);
        SizeType WriteString(StringView Str);
        SizeType WriteFloat(f32 Float);
        SizeType WriteDouble(double Double);

    protected:
        bool m_Opened = false;
        OpenModeFlags m_OpenMode = OpenModeFlagBits::None;
    };
}

