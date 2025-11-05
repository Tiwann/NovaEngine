#include "Containers/StringConversion.h"
#include <Windows.h>

namespace Nova
{
    WideString StringConvertToWide(const String& from)
    {
        const uint32_t length = MultiByteToWideChar(CP_UTF8, 0, from.Data(), from.Count(), nullptr, 0);
        WideString::CharacterType* buffer = new WideString::CharacterType[length] { 0 };
        MultiByteToWideChar(CP_UTF8, 0, from.Data(), from.Count(), buffer, length);
        WideString result { buffer, (size_t)length };
        delete[] buffer;
        return result;
    }

    String StringConvertToMultibyte(const WideString& from)
    {
        const uint32_t length = WideCharToMultiByte(CP_UTF8, 0, from.Data(), from.Count(), nullptr, 0, nullptr, nullptr);
        String::CharacterType* buffer = new String::CharacterType[length] { 0 };
        WideCharToMultiByte(CP_UTF8, 0, from.Data(), from.Count(), buffer, length, nullptr, nullptr);
        String result { buffer, (size_t)length };
        delete[] buffer;
        return result;
    }

    WideString StringConvertToWide(const StringView& from)
    {
        const uint32_t length = MultiByteToWideChar(CP_UTF8, 0, from.Data(), from.Count(), nullptr, 0);
        WideString::CharacterType* buffer = new WideString::CharacterType[length] { };
        MultiByteToWideChar(CP_UTF8, 0, from.Data(), from.Count(), buffer, length);
        WideString result { buffer, (size_t)length };
        delete[] buffer;
        return result;
    }

    String StringConvertToMultibyte(const WideStringView& from)
    {
        const uint32_t length = WideCharToMultiByte(CP_UTF8, 0, from.Data(), from.Count(), nullptr, 0, nullptr, nullptr);
        String::CharacterType* buffer = new String::CharacterType[length] { };
        WideCharToMultiByte(CP_UTF8, 0, from.Data(), from.Count(), buffer, length, nullptr, nullptr);
        String result { buffer, (size_t)length };
        delete[] buffer;
        return result;
    }
}
