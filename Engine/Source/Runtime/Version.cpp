#include "Version.h"

namespace Nova
{
    u32 Version::GetPacked() const
    {
        return (u32)Major << 16 | Minor;
    }

    int Version::Compare(const Version& Other) const
    {
        if (Major < Other.Major)
            return -1;
        if (Major > Other.Major)
            return 1;
        if (Major == Other.Major && Minor < Other.Minor)
            return -1;
        if (Major == Other.Major && Minor > Other.Minor)
            return 1;
        return 0;
    }
}
