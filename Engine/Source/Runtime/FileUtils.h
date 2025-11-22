#pragma once
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "IO/Stream.h"

namespace Nova::FileUtils
{
    Array<uint8_t> ReadToBuffer(StringView filepath);
    String ReadToString(StringView filepath);
    Array<uint8_t> ReadTillEnd(Stream& stream);
}
