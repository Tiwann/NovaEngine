#pragma once
#include "Interfaces/Equatable.h"

namespace Nova
{
    template<typename KeyType, typename ValueType>
    struct Pair : IEquatable<Pair<KeyType, ValueType>>
    {
        KeyType Key;
        ValueType Value;

        bool Equals(const Pair& Other) const override
        {
            return Key == Other.Key && Value == Other.Value;
        }
    };
}
