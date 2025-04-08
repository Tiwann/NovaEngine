#pragma once

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
}
