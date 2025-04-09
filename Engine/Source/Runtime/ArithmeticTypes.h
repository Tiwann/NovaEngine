#pragma once
#include "Runtime/Types.h"
#include "Runtime/TypeTraits.h"
#include "Interfaces/Equatable.h"
#include "Interfaces/Comparable.h"

namespace Nova
{
    template <typename T> requires IsArithmeticValue<T>
    struct ArithmeticType
    {
        T Value;

        ArithmeticType(const T Val) : Value(Val) {}
        operator T() const { return Value; }


        ArithmeticType operator+(const ArithmeticType& Other) const
        {
            return ArithmeticType(Value + Other.Value);
        }

        ArithmeticType operator+(const T& Other) const
        {
            return ArithmeticType(Value + Other);
        }

        friend ArithmeticType operator+(const T& Lhs, const ArithmeticType& Rhs)
        {
            return ArithmeticType(Lhs + Rhs.Value);
        }


        ArithmeticType& operator+=(const ArithmeticType& Other)
        {
            Value += Other.Value;
            return *this;
        }

        ArithmeticType& operator+=(const T& Other)
        {
            Value += Other;
            return *this;
        }

        ArithmeticType operator-(const ArithmeticType& Other) const
        {
            return ArithmeticType(Value - Other.Value);
        }

        ArithmeticType operator-(const T& Other) const
        {
            return ArithmeticType(Value - Other);
        }

        friend ArithmeticType operator-(const T& Lhs, const ArithmeticType& Rhs)
        {
            return ArithmeticType(Lhs - Rhs.Value);
        }

        ArithmeticType& operator-=(const ArithmeticType& Other)
        {
            Value -= Other.Value;
            return *this;
        }

        ArithmeticType& operator-=(const T& Other)
        {
            Value -= Other;
            return *this;
        }

        ArithmeticType& operator-()
        {
            Value = -Value;
            return *this;
        }

        ArithmeticType operator*(const ArithmeticType& Other) const
        {
            return ArithmeticType(Value * Other.Value);
        }

        ArithmeticType operator*(const T& Other) const
        {
            return ArithmeticType(Value * Other);
        }

        friend ArithmeticType operator*(const T& Lhs, const ArithmeticType& Rhs)
        {
            return ArithmeticType(Lhs * Rhs.Value);
        }

        ArithmeticType& operator*=(const ArithmeticType& Other)
        {
            Value *= Other.Value;
            return *this;
        }

        ArithmeticType& operator*=(const T& Other)
        {
            Value *= Other;
            return *this;
        }

        ArithmeticType operator/(const ArithmeticType& Other) const
        {
            return ArithmeticType(Value / Other.Value);
        }

        ArithmeticType operator/(const T& Other) const
        {
            return ArithmeticType(Value / Other);
        }

        friend ArithmeticType operator/(const T& Lhs, const ArithmeticType& Rhs)
        {
            return ArithmeticType(Lhs / Rhs.Value);
        }

        ArithmeticType& operator/=(const ArithmeticType& Other)
        {
            Value /= Other.Value;
            return *this;
        }

        ArithmeticType& operator/=(const T& Other)
        {
            Value /= Other;
            return *this;
        }
    };

    template <typename T> requires IsIntegerValue<T>
    struct Integer : ArithmeticType<T>, IEquatable<Integer<T>>, IComparable<Integer<T>>
    {
        using Base = ArithmeticType<T>;
        Integer(const T Val) : Base(Val) {}


        bool Equals(const Integer& Other) const override
        {
            return Base::Value == Other.Value;
        }

        int Compare(const Integer& Other) const override
        {
            if (Base::Value < Other.Value)
                return -1;
            if (Base::Value > Other.Value)
                return 1;
            return 0;
        }

        Integer operator%(const Integer& Other) const
        {
            return Integer(Base::Value % Other.Value);
        }

        Integer operator%(const T& Other) const
        {
            return Integer(Base::Value % Other);
        }

        friend Integer operator%(const T& Lhs, const Integer& Rhs)
        {
            return Integer(Lhs % Rhs.Value);
        }

        Integer& operator%=(const Integer& Other)
        {
            Base::Value %= Other.Value;
            return *this;
        }

        Integer& operator%=(const T& Other)
        {
            Base::Value %= Other;
            return *this;
        }

        Integer operator++() const
        {
            ++Base::Value;
            return *this;
        }

        Integer operator--() const
        {
            --Base::Value;
            return *this;
        }

    };

    template <typename T> requires IsFloatingPointValue<T>
    struct FloatingPoint : ArithmeticType<T>
    {
        using Base = ArithmeticType<T>;
        FloatingPoint(const T Val) : Base(Val) {}
    };

    struct Int8 final   : Integer<i8>{ Int8(const i8& Val) : Integer(Val) {} };
    struct Int16 final  : Integer<i16>{ Int16(const i16& Val) : Integer(Val) {} };
    struct Int32 final  : Integer<i32>{ Int32(const i32& Val) : Integer(Val) {} };
    struct Int64 final  : Integer<i64>{ Int64(const i64& Val) : Integer(Val) {} };
    struct UInt8 final  : Integer<u8>{ UInt8(const u8& Val) : Integer(Val) {} };
    struct UInt16 final : Integer<u16>{ UInt16(const u16& Val) : Integer(Val) {} };
    struct UInt32 final : Integer<u32>{ UInt32(const u32& Val) : Integer(Val) {} };
    struct UInt64 final : Integer<u64>{ UInt64(const u64& Val) : Integer(Val) {} };
    struct Float32 final : FloatingPoint<f32> { Float32(const f32& Val) : FloatingPoint(Val) {} };
    struct Float64 final : FloatingPoint<f64> { Float64(const f64& Val) : FloatingPoint(Val) {} };
}
