#pragma once
#include "String.h"
#include <imgui/imgui_internal.h>

namespace Nova
{
   
    template<typename FromChar, typename ToChar> requires (IsCharacter<FromChar>::Value && IsCharacter<ToChar>::Value)
    StringBase<ToChar> Convert(const StringBase<FromChar>& FromString)
    {
        return StringBase<ToChar>();
    }

    template<>
    StringBase<wchar_t> Convert(const StringBase<char>& FromString)
    {
        u8* Buffer = nullptr;
        
        const char* Begin = FromString.begin();
        const char* End = FromString.end();
        
    }
}
