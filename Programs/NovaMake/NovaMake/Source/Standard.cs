using System;

namespace NovaMake;

public enum CStandard
{
    None,
    C11,
    C17,
    CLatest
}

public enum CPPStandard
{
    None,
    CPP23,
    CPP20,
    CPP17,
    CPP14,
    CPP11,
    CPPLatest,
}

public enum CSharpVersion
{
    None,
    CSharp8,
    CSharp9,
    CSharp10,
    CSharp11,
    CSharp12,
    CSharpLatest
}



public static class DialectHelpers
{
    
    public static string GetString(this CStandard Standard)
    {
        return Standard switch
        {
            CStandard.C11 => "C11",
            CStandard.C17 => "C17",
            CStandard.CLatest => "CLatest",
            _ => throw new ArgumentOutOfRangeException(nameof(Standard), Standard, null)
        };
    }
    
    public static string GetString(this CPPStandard Standard)
    {
        return Standard switch
        {
            CPPStandard.CPP20 => "C++20",
            CPPStandard.CPP17 => "C++17",
            CPPStandard.CPP14 => "C++14",
            CPPStandard.CPP11 => "C++11",
            CPPStandard.CPPLatest => "C++ Latest",
            _ => throw new ArgumentOutOfRangeException(nameof(Standard), Standard, null)
        };
    }
    
    
    
    public static string GetString(this CSharpVersion Version)
    {
        return Version switch
        {
            CSharpVersion.CSharp8 => "C#8",
            CSharpVersion.CSharp9 => "C#9",
            CSharpVersion.CSharp10 => "C#10",
            CSharpVersion.CSharp11 => "C#11",
            CSharpVersion.CSharpLatest => "C#Latest",
            _ => throw new ArgumentOutOfRangeException(nameof(Version), Version, null)
        };
    }
}