#pragma once
#include "TypeTraits.h"
#include <type_traits>

#define NOVA_ENUM_TYPE_V(Type, Value) std::underlying_type_t<Type>(Value)
#define NOVA_FLAG_APPEND(Left, Right) ((Left) | (Right))
#define NOVA_FLAG_REMOVE(Left, Right) ((Left) & ~(Right))
#define NOVA_FLAG_BIT(Name, Position) Name = 1 << Position

namespace Nova
{
    consteval unsigned int BIT(const unsigned int Position)
    {
        return 1 << Position;
    }
}

#define NOVA_DECLARE_FLAGS(Enum, Name) \
    using Name = Nova::Flags<Enum>; \
    \
    inline static Enum operator|(Enum Lhs, Enum Rhs) \
    { \
        return static_cast<Enum>(NOVA_ENUM_TYPE_V(Enum, Lhs) | NOVA_ENUM_TYPE_V(Enum, Rhs)); \
    } \
    \
    inline static Enum operator&(Enum Lhs, Enum Rhs) \
    { \
        return static_cast<Enum>(NOVA_ENUM_TYPE_V(Enum, Lhs) & NOVA_ENUM_TYPE_V(Enum, Rhs)); \
    }


namespace Nova
{
    template <typename Enum> requires IsEnumValue<Enum>
    struct Flags
    {
        Flags() : m_Value((Enum)0){}
        Flags(Enum Val) : m_Value(Val){}
        explicit Flags(u32 Val) : m_Value((Enum)Val){}
        
        Flags Append(Flags Value)
        {
            m_Value = (Enum)NOVA_FLAG_APPEND(NOVA_ENUM_TYPE_V(Enum, m_Value), NOVA_ENUM_TYPE_V(Enum, Value.m_Value));
            return m_Value;
        }

        Flags Remove(Flags Value)
        {
            m_Value = (Enum)NOVA_FLAG_REMOVE(NOVA_ENUM_TYPE_V(Enum, m_Value), NOVA_ENUM_TYPE_V(Enum, Value.m_Value));
            return m_Value;
        }

        Flags Toggle(Flags Value)
        {
            return Contains(Value) ? Remove(Value) : Append(Value);
        }

        bool Contains(Flags Value) const
        {
            return static_cast<bool>(NOVA_ENUM_TYPE_V(Enum, m_Value) & NOVA_ENUM_TYPE_V(Enum, Value.m_Value));
        }

        template<typename T> requires IsIntegerValue<T>
        T As() const { return (T)NOVA_ENUM_TYPE_V(Enum, m_Value); }

        bool operator==(const Enum& Other) const
        {
            return m_Value == Other;
        }

        Flags operator|(Flags Other)
        {
            return Append(Other);
        }

        Flags& operator|=(Flags Other)
        {
            Append(Other);
            return *this;
        }
        
        operator Enum()
        {
            return m_Value;
        }

        

    private:
        Enum m_Value = 0; 
    };
    
}

    