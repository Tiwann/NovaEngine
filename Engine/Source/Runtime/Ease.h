#pragma once
#include <concepts>
#include <cmath>
#include <numbers>

namespace
{
    template<std::floating_point T>
    constexpr T SQUARE(T t) { return t * t; }

    template<std::floating_point T>
    constexpr T CUBE(T t) { return t * t * t; }

    template<std::floating_point T>
    constexpr T QUART(T t) { return t * t * t * t; }
}

namespace Nova
{
    enum class EaseType
    {
        Linear,
        QuadraticIn,
        QuadraticOut,
        QuadraticInOut,
        CubicIn,
        CubicOut,
        CubicInOut,
        QuarticIn,
        QuarticOut,
        QuarticInOut,
        ExpoIn,
        ExpoOut,
        ExpoInOut,
        BackIn,
        BackOut,
        BackInOut,
        SineIn,
        SineOut,
        SineInOut,
    };

    template<std::floating_point T = float>
    using EaseFunction = T(*)(T);

    template <std::floating_point T = float>
    static constexpr EaseFunction<T> GetEaseFunction(EaseType easeType)
    {
        switch (easeType)
        {
        case EaseType::Linear:
            {
                static constexpr auto f = [](T t) constexpr { return t; };
                return f;
            }
        case EaseType::QuadraticIn:
            {
                static constexpr auto f = [](T t) constexpr { return SQUARE(t); };
                return f;
            }
        case EaseType::QuadraticOut:
            {
                static constexpr auto f = [](T t) constexpr{return T(1) - SQUARE(T(1) - t);};
                return f;
            }
        case EaseType::QuadraticInOut:
            {
                static constexpr auto f = [](T t) constexpr{ return t < 0.5? 2 * SQUARE(t): 1 - SQUARE(-2 * t + 2) / 2; };
                return f;
            }
        case EaseType::CubicIn:
            {
                static constexpr auto f = [](T t) constexpr { return CUBE(t); };
                return f;
            }
        case EaseType::CubicOut:
            {
                static constexpr auto f = [](T t) constexpr{return T(1) - CUBE(T(1) - t);};
                return f;
            }
        case EaseType::CubicInOut:
            {
                static constexpr auto f = [](T t) constexpr { return t < 0.5 ? 4 * CUBE(t) : 1 - CUBE(-2 * t + 2) / 2; };
                return f;
            }
        case EaseType::QuarticIn:
            {
                static constexpr auto f = [](T t) constexpr { return QUART(t); };
                return f;
            }
        case EaseType::QuarticOut:
            {
                static constexpr auto f = [](T t) constexpr { return T(1) - QUART(T(1) - t); };
                return f;
            }
        case EaseType::QuarticInOut:
            {
                static constexpr auto f = [](T t) constexpr { return t < 0.5 ? 8 * QUART(t) : 1 - QUART(-2 * t + 2) / 2; };
                return f;
            }
        case EaseType::ExpoIn:
            {
                static constexpr auto f = [](T t) constexpr { return t == 0 ? 0 : std::exp2(10 * t - 10); };
                return f;
            }
        case EaseType::ExpoOut:
            {
                static constexpr auto f = [](T t) constexpr { return t == 1 ? 1 : T(1) - std::exp2(-10 * t); };
                return f;
            }
        case EaseType::ExpoInOut:
            {
                static constexpr auto f = [](T t) constexpr
                {
                    if (t == 0) return T(0);
                    if (t == 1) return T(1);
                    return t < 0.5
                               ? std::exp2(20 * t - 10) / 2
                               : (2 - std::exp2(-20 * t + 10)) / 2;
                };
                return f;
            }
        case EaseType::BackIn:
            {
                static constexpr auto f = [](T t) constexpr
                {
                    const T c1 = 1.70158;
                    const T c3 = c1 + 1;
                    return c3 * t * t * t - c1 * t * t;
                };
                return f;
            }
        case EaseType::BackOut:
            {
                static constexpr auto f = [](T t) constexpr
                {
                    const T c1 = 1.70158;
                    const T c3 = c1 + 1;
                    T u = t - 1;
                    return 1 + c3 * u * u * u + c1 * u * u;
                };
                return f;
            }
        case EaseType::BackInOut:
            {
                static constexpr auto f = [](T t) constexpr
                {
                    const T c1 = 1.70158;
                    const T c2 = c1 * 1.525;
                    return t < 0.5
                               ? ((2 * t) * (2 * t) * ((c2 + 1) * 2 * t - c2)) / 2
                               : (((2 * t - 2) * (2 * t - 2)) * ((c2 + 1) * (2 * t - 2) + c2) + 2) / 2;
                };
                return f;
            }
        case EaseType::SineIn:
            {
                static constexpr auto f = [](T t) constexpr
                {
                    return 1 - std::cos((t * std::numbers::pi_v<T>) / 2);
                };
                return f;
            }
        case EaseType::SineOut:
            {
                static constexpr auto f = [](T t) constexpr
                {
                    return std::sin((t * std::numbers::pi_v<T>) / 2);
                };
                return f;
            }
        case EaseType::SineInOut:
            {
                static constexpr auto f = [](T t) constexpr
                {
                    return -(std::cos(std::numbers::pi_v<T> * t) - 1) / 2;
                };
                return f;
            }
        default:
            return nullptr;
        }
    }
}
