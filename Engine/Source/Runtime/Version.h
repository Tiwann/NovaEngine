#pragma once
#include "NumericTypes.h"

namespace Nova
{
    struct Version
    {
        u16 Major = 0, Minor = 0;
        u32 GetPacked() const;
        bool operator<(const Version& Other) const;
        bool operator==(const Version& Other) const;
    };
}