using System;

namespace NovaMake;

public enum Language
{
    C,
    CPlusPlus,
    CSharp
}

public static class LanguageHelpers
{
    public static bool IsC(this Language Language)
    {
        return Language is Language.C or Language.CPlusPlus;
    }

    public static string GetString(this Language Language)
    {
        switch (Language)
        {
            case Language.C:
                return "C";
            case Language.CPlusPlus:
                return "C++";
            case Language.CSharp:
                return "C#";
            default:
                throw new ArgumentOutOfRangeException(nameof(Language), Language, null);
        }
    }
}