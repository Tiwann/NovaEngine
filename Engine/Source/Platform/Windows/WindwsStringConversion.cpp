#include "Containers/StringConversion.h"
#include <Windows.h>

namespace Nova
{
    WideString StringConvertToWide(const String& From)
    {
        const i32 Length = MultiByteToWideChar(CP_UTF8, 0, From.Data(), From.Count(), nullptr, 0);
        WideString::CharacterType* Buffer = new WideString::CharacterType[Length];
        MultiByteToWideChar(CP_UTF8, 0, From.Data(), From.Count(), Buffer, Length);
        WideString Result { Buffer };
        delete[] Buffer;
        return Result;
    }

    String StringConvertToMultibyte(const WideString& From)
    {
        const i32 Length = WideCharToMultiByte(CP_UTF8, 0, From.Data(), From.Count(), nullptr, 0, nullptr, nullptr);
        String::CharacterType* Buffer = new String::CharacterType[Length];
        WideCharToMultiByte(CP_UTF8, 0, From.Data(), From.Count(), Buffer, Length, nullptr, nullptr);
        String Result { Buffer, (size_t)Length };
        delete[] Buffer;
        return Result;
    }

    WideString StringConvertToWide(const StringView& From)
    {
        const i32 Length = MultiByteToWideChar(CP_UTF8, 0, From.Data(), From.Count(), nullptr, 0);
        WideString::CharacterType* Buffer = new WideString::CharacterType[Length];
        MultiByteToWideChar(CP_UTF8, 0, From.Data(), From.Count(), Buffer, Length);
        WideString Result { Buffer };
        delete[] Buffer;
        return Result;
    }

    String StringConvertToMultibyte(const WideStringView& From)
    {
        const i32 Length = WideCharToMultiByte(CP_UTF8, 0, From.Data(), From.Count(), nullptr, 0, nullptr, nullptr);
        String::CharacterType* Buffer = new String::CharacterType[Length];
        WideCharToMultiByte(CP_UTF8, 0, From.Data(), From.Count(), Buffer, Length, nullptr, nullptr);
        String Result { Buffer, (size_t)Length };
        delete[] Buffer;
        return Result;
    }
}
