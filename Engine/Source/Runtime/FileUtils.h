#pragma once
#include "Containers/String.h"
#include "Containers/StringView.h"

namespace Nova::File
{
    String ReadToString(const StringView filepath);
}
