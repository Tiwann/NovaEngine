using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace NovaMake;

public static class Extenders
{
    public static bool IsSubclassOf<T>(this Type Type)
    {
        return Type.IsSubclassOf(typeof(T));
    }

    public static T CreateInstance<T>(this Type Type, params dynamic[] Parameters) where T : class
    {
        return Type.IsSubclassOf<T>() ? Activator.CreateInstance(Type, Parameters) as T : null;
    }

    public static T Last<T>(this IEnumerable<T> List) where T : class
    {
        var Temp = List.ToList();
        return Temp.Count != 0 ? Temp[^1] : null;
    }

    public static bool IsEmpty<T>(this IEnumerable<T> List)
    {
        return List.Count() == 0;
    }

    public static void AddRange<T>(this List<T> List, params T[] Objects)
    {
        List.AddRange(new List<T>(Objects));
    }

    public static bool Contains<T>(this T TheEnum, T Value) where T : Enum
    {
        return Convert.ToBoolean(Convert.ToInt32(TheEnum) & Convert.ToInt32(Value));
    }
    
    public static string GetRelativePath(this string FromPath, string ToPath)
    {
        if (string.IsNullOrEmpty(FromPath))
        {
            throw new ArgumentNullException($"{FromPath}");
        }

        if (string.IsNullOrEmpty(ToPath))
        {
            throw new ArgumentNullException($"{ToPath}");
        }

        Uri FromUri = new Uri(AppendDirectorySeparatorChar(FromPath));
        Uri ToUri = new Uri(AppendDirectorySeparatorChar(ToPath));

        if (FromUri.Scheme != ToUri.Scheme)
        {
            return ToPath;
        }

        Uri RelativeUri = FromUri.MakeRelativeUri(ToUri);
        string RelativePath = Uri.UnescapeDataString(RelativeUri.ToString());

        if (string.Equals(ToUri.Scheme, Uri.UriSchemeFile, StringComparison.OrdinalIgnoreCase))
        {
            RelativePath = RelativePath.Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar);
        }

        return RelativePath;
    }

    private static string AppendDirectorySeparatorChar(string path)
    {
        if (!Path.HasExtension(path) &&
            !path.EndsWith(Path.DirectorySeparatorChar.ToString()))
        {
            return path + Path.DirectorySeparatorChar;
        }

        return path;
    }

    public static bool DirectoryIsEmpty(this string Path)
    {
        var Directories = Directory.EnumerateDirectories(Path);
        var Files = Directory.EnumerateFiles(Path);
        return Directories.IsEmpty() && Files.IsEmpty();
    }
}