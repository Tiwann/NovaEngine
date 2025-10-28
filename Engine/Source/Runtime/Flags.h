#pragma once
#include <cstdint>
#include <type_traits>

#define NOVA_DECLARE_FLAGS(enumClass, name) using name = ::Nova::Flags<enumClass>

namespace Nova
{
    consteval size_t BIT(const size_t N)
    {
        return 1 << N;
    }

    template<typename Enum> requires std::is_scoped_enum_v<Enum>
    class Flags
    {
    public:
        constexpr Flags() = default;
        constexpr Flags(Enum value) : m_Value(value) {}
        constexpr Flags(uint32_t value) : m_Value((Enum)value) {}

        static constexpr Flags All() {  return 0xFFFFFFFF; }

        constexpr operator uint32_t() const { return (uint32_t)m_Value; }

        constexpr bool operator==(const Flags& other) const { return m_Value == other.m_Value; }

        constexpr Flags operator|(const Enum& other) const { return Flags((uint32_t)m_Value | (uint32_t)other); };
        constexpr Flags operator&(const Enum& other) const { return Flags((uint32_t)m_Value & (uint32_t)other); };
        constexpr Flags operator^(const Enum& other) const { return Flags((uint32_t)m_Value ^ (uint32_t)other); };
        constexpr Flags& operator|=(const Enum& other) { m_Value = (Enum)((uint32_t)m_Value | (uint32_t)other); return *this; };
        constexpr Flags& operator&=(const Enum& other) { m_Value = (Enum)((uint32_t)m_Value & (uint32_t)other); return *this; };
        constexpr Flags& operator^=(const Enum& other) { m_Value = (Enum)((uint32_t)m_Value ^ (uint32_t)other); return *this; };

        template<typename T> requires std::is_same_v<T, std::underlying_type_t<Enum>>
        T As() const { return (T)m_Value; }

        constexpr bool Contains(const Enum& other) const { return ((uint32_t)m_Value & (uint32_t)other) != 0; }

        constexpr Flags& Append(const Enum value)
        {
            m_Value = (Enum)((uint32_t)m_Value | (uint32_t)value);
            return *this;
        }

        constexpr Flags& Remove(const Enum value)
        {
            m_Value = (Enum)((uint32_t)m_Value & ~(uint32_t)value);
            return *this;
        }

        constexpr Flags& Toggle(const Enum value) { return Contains(value) ? Remove(value) : Append(value); }
    private:
        Enum m_Value = (Enum)0;
    };

    template<typename Enum> requires std::is_scoped_enum_v<Enum>
    constexpr Flags<Enum> operator|(const Enum& lhs, const Enum& rhs)
    {
        return Flags((Enum)((uint32_t)lhs | (uint32_t)rhs));
    }

    template<typename Enum> requires std::is_scoped_enum_v<Enum>
    constexpr Flags<Enum> operator&(const Enum& lhs, const Enum& rhs)
    {
        return Flags((Enum)((uint32_t)lhs & (uint32_t)rhs));
    }

    template<typename Enum> requires std::is_scoped_enum_v<Enum>
    constexpr Flags<Enum> operator^(const Enum& lhs, const Enum& rhs)
    {
        return Flags((Enum)((uint32_t)lhs ^ (uint32_t)rhs));
    }
}
