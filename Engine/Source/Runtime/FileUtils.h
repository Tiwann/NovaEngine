#pragma once
#include "Containers/String.h"
#include "Containers/StringView.h"

namespace Nova::FileUtils
{
    Array<uint8_t> ReadToBuffer(const StringView filepath);
    String ReadToString(const StringView filepath);
}
