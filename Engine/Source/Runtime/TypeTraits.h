#pragma once
#include "Color.h"


namespace Nova
{
    template<bool v>
    struct BooleanConstant
    {
        static constexpr bool value = v;
    };
    
    struct TrueType : BooleanConstant<true> {};
    
    struct FalseType : BooleanConstant<false> {};

    template<typename T>
    struct IsInteger : FalseType{};

    template<>
    struct IsInteger<int8_t> : TrueType{};

    template<>
    struct IsInteger<int16_t> : TrueType{};

    template<>
    struct IsInteger<int32_t> : TrueType{};

    template<>
    struct IsInteger<int64_t> : TrueType{};


    template<>
    struct IsInteger<uint8_t> : TrueType{};

    template<>
    struct IsInteger<uint16_t> : TrueType{};

    template<>
    struct IsInteger<uint32_t> : TrueType{};

    template<>
    struct IsInteger<uint64_t> : TrueType{};
    
    template<>
    struct IsInteger<long> : TrueType{};
    

    template<typename T>
    static inline constexpr bool IsIntegerValue = IsInteger<T>::value;


    template<typename T>
    struct IsFloatingPoint : FalseType {};
    
    template<>
    struct IsFloatingPoint<float> : TrueType{};

    template<>
    struct IsFloatingPoint<double> : TrueType{};

    template<typename T>
    static inline constexpr bool IsFloatingPointValue = IsFloatingPoint<T>::value;

    template<typename T>
    static inline constexpr bool IsArithmeticValue = IsIntegerValue<T> || IsFloatingPointValue<T>;


    template<typename T>
    struct IsVector : FalseType {};
    
    template<>
    struct IsVector<struct Vector2> : TrueType{};

    template<>
    struct IsVector<struct Vector3> : TrueType{};

    template<>
    struct IsVector<struct Vector4> : TrueType{};

    template<typename T>
    static constexpr bool IsVectorValue = IsVector<T>::value;

    template<typename T>
    struct IsMatrix : FalseType {};

    template<>
    struct IsMatrix<class Matrix2> : TrueType{};

    template<>
    struct IsMatrix<class Matrix3> : TrueType{};

    template<>
    struct IsMatrix<class Matrix4> : TrueType{};

    template<typename T>
    struct IsQuaternion : FalseType {};

    template<>
    struct IsQuaternion<struct Quaternion> : TrueType{};

    template<typename T>
    static constexpr bool IsQuaternionValue = IsQuaternion<T>::value;

    template<typename T>
    static constexpr bool IsMatrixValue = IsMatrix<T>::value;
    
    template<typename Base, typename Derived>
    struct IsBaseOf : BooleanConstant<__is_base_of(Base, Derived)> {};

    template<typename Base, typename Derived>
    static constexpr bool IsBaseOfValue = IsBaseOf<Base, Derived>::value;

    template<typename T>
    struct IsEnum : BooleanConstant<__is_enum(T)> {};

    template<typename T>
    static constexpr bool IsEnumValue = IsEnum<T>::value;
    
    template<typename T>
    struct IsCharacter : FalseType {};

    template<>
    struct IsCharacter<char> : TrueType {};

    template<>
    struct IsCharacter<char8_t> : TrueType {};

    template<>
    struct IsCharacter<char16_t> : TrueType {};

    template<>
    struct IsCharacter<char32_t> : TrueType {};

    template<>
    struct IsCharacter<wchar_t> : TrueType {};
    
    template<typename T>
    static inline constexpr bool IsCharacterValue = IsCharacter<T>::value;


    template <typename, typename>
    static inline constexpr bool IsSameValue = false;
    
    template <typename T>
    static inline constexpr bool IsSameValue<T, T> = true;

    template <typename T, typename U>
    struct IsSame : BooleanConstant<IsSameValue<T, U>>{};

    template<typename T>
    struct IsColor : FalseType {};

    template<>
    struct IsColor<Color> : TrueType {};

    template<typename T>
    static constexpr bool IsColorValue = IsColor<T>::value;

    template<typename From, typename To>
    static constexpr bool IsConvertible = __is_convertible_to(From, To);
}
