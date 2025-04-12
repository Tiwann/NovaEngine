#pragma once
#include "String.h"
#include "StringView.h"

namespace Nova
{
    WideString StringConvertToWide(const String& From);
    String StringConvertToMultibyte(const WideString& From);
    WideString StringConvertToWide(const StringView& From);
    String StringConvertToMultibyte(const WideStringView& From);
}
