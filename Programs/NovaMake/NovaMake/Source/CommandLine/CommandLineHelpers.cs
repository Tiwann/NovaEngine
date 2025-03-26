namespace NovaMake;

public static class DictionaryHelpers
{
    public static void Add<T, U>(this Dictionary<T, U> Dictionary, (T, U) Tuple) where T : notnull
    {
        Dictionary.Add(Tuple.Item1, Tuple.Item2);
    }

    public static void Add<T, U>(this Dictionary<T, U> Dictionary, Tuple<T, U> Tuple) where T : notnull
    {
        Dictionary.Add(Tuple.Item1, Tuple.Item2);
    }
}