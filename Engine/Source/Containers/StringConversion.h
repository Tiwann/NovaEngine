#pragma once
#include "String.h"
#include "StringView.h"

namespace Nova
{
    WideString StringConvertToWide(const String& from);
    String StringConvertToMultibyte(const WideString& from);
    WideString StringConvertToWide(const StringView& from);
    String StringConvertToMultibyte(const WideStringView& from);
}
