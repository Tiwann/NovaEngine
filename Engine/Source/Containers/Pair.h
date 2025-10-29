#pragma once

namespace Nova
{
    template<typename KeyType, typename ValueType>
    struct Pair
    {
        KeyType key;
        ValueType value;

        bool operator==(const Pair& other) const
        {
            return key == other.key && value == other.value;
        }
    };
}
