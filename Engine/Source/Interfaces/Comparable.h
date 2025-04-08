#pragma once

namespace Nova
{
    template <typename T>
    struct IComparable
    {
        virtual ~IComparable() = default;
        virtual int Compare(const T& Other) const = 0;

        bool operator<(const T& Other) const
        {
            return Compare(Other) < 0;
        }

        bool operator>(const T& Other) const
        {
            return Compare(Other) > 0;
        }

        bool operator==(const T& Other) const
        {
            return Compare(Other) == 0;
        }
    };
}
