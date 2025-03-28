#pragma once
#pragma warning(disable:4146)
#include "Pair.h"
#include "Array.h"


namespace Nova
{
    template<typename KeyType, typename ValueType>
    class Map : Iterable<Pair<KeyType, ValueType>>
    {
    public:
        using PairType = Pair<KeyType, ValueType>;
        using ArrayType = Array<PairType>;
        using SizeType = typename Array<PairType>::SizeType;
        using Iterator = Iterator<PairType>;
        using ConstIterator = ConstIterator<PairType>;
        
        Map() = default;
        Map(const std::initializer_list<PairType>& Array) : m_Data(ArrayType(Array)){}
        Map(const Map&) = default;
        Map(Map&&) noexcept = default;
        Map& operator=(const Map&) = default;
        Map& operator=(Map&&) = default;

        
        ValueType& operator[](const KeyType& Key)
        {
            SizeType Index = FindKey(Key);
            if(Index == -1)
            {
                const PairType Pair{ .Key = Key };
                m_Data.Add(Pair);
                return m_Data.Last().Value;
            }
            return m_Data[Index].Value;
        }

        const ValueType& operator[](const KeyType& Key) const
        {
            SizeType Index = FindKey(Key);
            if(Index == -1)
            {
                const PairType Pair{ .Key = Key };
                m_Data.Add(Pair);
                return m_Data.Last().Value;
            }
            return m_Data[Index].Value;
        }

        SizeType FindKey(const KeyType& Key)
        {
            for(SizeType i = 0; i < m_Data.Count(); ++i)
            {
                const PairType& Pair = m_Data.GetAt(i);
                if(Key == Pair.Key)
                    return i;
            }
            return -1;
        }

        SizeType FindValue(const ValueType& Value)
        {
            for(SizeType i = 0; i < m_Data.Count(); ++i)
            {
                const PairType& Pair = m_Data.GetAt(i);
                if(Value == Pair.Value)
                    return i;
            }
            return -1;
        }

        bool Contains(const KeyType& Key)
        {
            return FindKey(Key) != -1ULL;
        }

        SizeType Count() const { return m_Data.Count(); }

        PairType& GetAt(SizeType Index)
        {
            return m_Data.GetAt(Index);
        }

        const PairType& GetAt(SizeType Index) const
        {
            return m_Data.GetAt(Index);    
        }

        void Clear()
        {
            m_Data.Clear();
        }

        bool Remove(const PairType& Pair)
        {
            if(m_Data.Contains(Pair))
            {
                m_Data.Remove(Pair);
                return true;
            }
            return false;
        }

        bool operator==(const Map& Other) const
        {
            return m_Data == Other.m_Data;
        }
        
        
        Iterator begin() override { return m_Data.begin(); }
        Iterator end() override { return m_Data.end(); }
        ConstIterator begin() const override { return m_Data.begin(); }
        ConstIterator end() const override { return m_Data.end(); }
    
    private:
        ArrayType m_Data;
    };
}
