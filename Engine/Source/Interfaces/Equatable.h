#pragma once
#include "Runtime/Memory.h"

namespace Nova
{
    template<typename T>
    struct IEquatable
    {
        virtual ~IEquatable() = default;
        virtual bool Equals(const T& Other) const = 0;
        bool operator==(const T& Other) const
        {
            return Equals(Other);
        }

        bool operator!=(const T& Other) const
        {
            return !Equals(Other);
        }
    };

    template<typename T>
    struct IStructEquatable : IEquatable<T>
    {
        bool Equals(const T& Other) const override
        {
            return Memory::Memcmp(this, &Other, sizeof(T));
        }
    };
}
