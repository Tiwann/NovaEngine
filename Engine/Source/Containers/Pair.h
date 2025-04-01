#pragma once

namespace Nova
{
    template<typename KeyType, typename ValueType>
    struct Pair
    {
        KeyType Key;
        ValueType Value;

        bool operator==(const Pair& Other) const
        {
            return Key == Other.Key && Value == Other.Value;
        }
    };
}
