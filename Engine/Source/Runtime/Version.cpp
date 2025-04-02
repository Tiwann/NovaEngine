#include "Version.h"

namespace Nova
{
    u32 Version::GetPacked() const
    {
        return (u32)Major << 16 | Minor;
    }
    
    bool Version::operator<(const Version& Other) const
    {
        if (Major < Other.Major)
            return true;
        if (Major > Other.Major)
            return false;
        if (Major == Other.Major && Minor < Other.Minor)
            return true;
        if (Major == Other.Major && Minor > Other.Minor)
            return false;
        return false;
    }
    
    bool Version::operator==(const Version& Other) const
    {
        return Major == Other.Major && Minor == Other.Minor;
    }
}
