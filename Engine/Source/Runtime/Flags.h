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
        Flags() = default;
        Flags(Enum value) : m_Value((uint32_t)value) {}

        operator uint32_t() const { return m_Value; }

        Flags operator|(const Flags& other) const { return Flags(m_Value | other.m_Value); };
        Flags operator&(const Flags& other) const { return Flags(m_Value & other.m_Value); };
        Flags operator^(const Flags& other) const { return Flags(m_Value ^ other.m_Value); };
        Flags operator~() const { return Flags(~m_Value); };
        Flags& operator|=(const Flags& other) { m_Value = m_Value | other.m_Value; return *this; };
        Flags& operator&=(const Flags& other) { m_Value = m_Value & other.m_Value; return *this; };
        Flags& operator^=(const Flags& other) { m_Value = m_Value ^ other.m_Value; return *this; };

        Flags operator|(const Enum& other) const { return Flags(m_Value | (uint32_t)other); };
        Flags operator&(const Enum& other) const { return Flags(m_Value & (uint32_t)other); };
        Flags operator^(const Enum& other) const { return Flags(m_Value ^ (uint32_t)other); };
        Flags& operator|=(const Enum& other) { m_Value = m_Value | (uint32_t)other; return *this; };
        Flags& operator&=(const Enum& other) { m_Value = m_Value & (uint32_t)other; return *this; };
        Flags& operator^=(const Enum& other) { m_Value = m_Value ^ (uint32_t)other; return *this; };

        template<typename T> requires std::is_same_v<T, std::underlying_type_t<Enum>>
        T As() const { return (T)m_Value; }

        bool Contains(const Enum& other) const { return m_Value & (uint32_t)other; }
    private:
        uint32_t m_Value = 0;
    };

    template<typename Enum> requires std::is_scoped_enum_v<Enum>
    Flags<Enum> operator|(const Enum& lhs, const Enum& rhs)
    {
        return Flags((Enum)((uint32_t)lhs | (uint32_t)rhs));
    }

    template<typename Enum> requires std::is_scoped_enum_v<Enum>
    Flags<Enum> operator&(const Enum& lhs, const Enum& rhs)
    {
        return Flags((Enum)((uint32_t)lhs & (uint32_t)rhs));
    }

    template<typename Enum> requires std::is_scoped_enum_v<Enum>
    Flags<Enum> operator^(const Enum& lhs, const Enum& rhs)
    {
        return Flags((Enum)((uint32_t)lhs ^ (uint32_t)rhs));
    }
}
