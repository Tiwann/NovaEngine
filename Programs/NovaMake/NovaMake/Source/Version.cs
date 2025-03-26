using System;

namespace NovaMake;
public class Version : IComparable<Version>
{
    public ushort Major { get; }
    public ushort Minor { get; }
    public ushort Revision { get; }

    public Version(ushort Maj, ushort Min, ushort Rev)
    {
        Major = Maj;
        Minor = Min;
        Revision = Rev;
    }

    public int CompareTo(Version Other)
    {
        int Result = 0;
        
        if (Major > Other.Major)
            Result = 1;
        if (Major == Other.Major && Minor > Other.Minor)
            Result = 1;
        if (Major == Other.Major && Minor == Other.Minor && Revision > Other.Revision)
            Result = 1;
        if (Major == Other.Major && Minor == Other.Minor && Revision == Other.Revision)
            Result = 0;
        if (Major < Other.Major)
            Result = -1;
        if (Major == Other.Major && Minor < Other.Minor)
            Result = -1;
        if (Major == Other.Major && Minor == Other.Minor && Revision < Other.Revision)
            Result = -1;
        return Result;
    }


    public override string ToString()
    {
        return $"{Major}.{Minor}.{Revision}";
    }

    public static readonly Version NovaVersion = new (1, 2, 2);
}

