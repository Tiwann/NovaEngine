using System;
using System.Collections.Generic;
using System.Linq;

namespace NovaMake;

public static class StringExtensions
{
    public static bool ToBool(this string value)
    {
        string[] TrueValues = { "true", "True", "TRUE", "1" };
        string[] FalseValues = { "false", "False", "FALSE", "0" };

        if (TrueValues.Any(val => val.Equals(value))) return true;
        if (FalseValues.Any(val => val.Equals(value))) return false;
        return false;
    }

    public static string FindAndTrimEnd(this string str, string sequence)
    {
        int SequencePosition = str.IndexOf(sequence, StringComparison.Ordinal);
        if (SequencePosition == -1) return string.Empty;

        return str.Substring(0, SequencePosition);
    }
    
    public static string FindAndTrimStart(this string str, string sequence)
    {
        int SequencePosition = str.IndexOf(sequence, StringComparison.Ordinal);
        if (SequencePosition == -1) return string.Empty;

        return str.Substring(SequencePosition);
    }

    public static string GetList(this IEnumerable<string> List, char Separator)
    {
        var Enumerable = List.ToList();
        if (Enumerable.IsEmpty()) return string.Empty;
        
        string Result = string.Empty;
        for (int i = 0; i < Enumerable.Count; i++)
        {
            Result += Enumerable[i];
            if (i != Enumerable.Count - 1)
                Result += Separator;
        }

        return Result;
    }
}