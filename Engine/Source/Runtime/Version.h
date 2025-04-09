#pragma once
#include "Types.h"
#include "Interfaces/Comparable.h"

namespace Nova
{
    struct Version : IComparable<Version>
    {
        u16 Major = 0, Minor = 0;
        Version(u16 Major = 0, u16 Minor = 0) : Major(Major), Minor(Minor) {}

        u32 GetPacked() const;
        int Compare(const Version& Other) const override;
    };
}