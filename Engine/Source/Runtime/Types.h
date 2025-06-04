#pragma once
#include <cstddef>

namespace Nova
{
    using f32 = float;
    using f64 = double;
    using u8  = unsigned char;
    using u16 = unsigned short;
    using u32 = unsigned int;
    using u64 = unsigned long long;
    using i8  = signed char;
    using i16 = signed short;
    using i32 = int;
    using i64 = long long;

    constexpr i8  I8_MAX = 0x7Fi8;
    constexpr i8  I8_MIN = -I8_MAX - 1;
    constexpr i16 I16_MAX = 0x7FFFi16;
    constexpr i16 I16_MIN = -I16_MAX - 1;
    constexpr i32 I32_MAX = 0x7FFFFFFFi32;
    constexpr i32 I32_MIN = -I32_MAX - 1;
    constexpr i64 I64_MAX = 0x7FFFFFFFFFFFFFFFi64;
    constexpr i64 I64_MIN = -I64_MAX - 1;

    constexpr u8  U8_MAX = 0xFFui8;
    constexpr u8  U8_MIN = -I8_MAX - 1;
    constexpr u16 U16_MAX = 0xFFFFui16;
    constexpr u16 U16_MIN = -I16_MAX - 1;
    constexpr u32 U32_MAX = 0xFFFFFFFFui32;
    constexpr u32 U32_MIN = -I32_MAX - 1;
    constexpr u64 U64_MAX = 0xFFFFFFFFFFFFFFFFui64;
    constexpr u64 U64_MIN = -I64_MAX - 1;
}