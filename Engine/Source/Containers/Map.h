#pragma once
#pragma warning(disable:4146)
#include "Pair.h"
#include "Array.h"
#include <map>

namespace Nova
{
    template<typename KeyType, typename ValueType>
    class Map
    {
    public:
        using PairType = Pair<KeyType, ValueType>;
        using ArrayType = Array<PairType>;
        using SizeType = Array<PairType>::SizeType;
        using Iterator = ArrayType::Iterator;
        using ConstIterator = ArrayType::ConstIterator;

        Map() = default;
        Map(const ArrayType& array) : m_Data(array){}
        Map(const Map&) = default;
        Map(Map&&) noexcept = default;
        Map& operator=(const Map&) = default;
        Map& operator=(Map&&) = default;


        ValueType& operator[](const KeyType& key)
        {
            SizeType index = FindKey(key);
            if(index == ~0)
            {
                const PairType pair{ .key = key };
                m_Data.Add(pair);
                return m_Data.Last().value;
            }
            return m_Data[index].value;
        }

        const ValueType& operator[](const KeyType& key) const
        {
            SizeType index = FindKey(key);
            NOVA_ASSERT(index != ~0, "Key not found");
            return m_Data[index].value;
        }

        SizeType FindKey(const KeyType& key) const
        {
            for(SizeType i = 0; i < m_Data.Count(); ++i)
            {
                const PairType& pair = m_Data.GetAt(i);
                if(key == pair.key)
                    return i;
            }
            return -1;
        }

        SizeType FindValue(const ValueType& value)
        {
            for(SizeType i = 0; i < m_Data.Count(); ++i)
            {
                const PairType& pair = m_Data.GetAt(i);
                if(value == pair.value)
                    return i;
            }
            return -1;
        }

        KeyType GetKey(const ValueType& value) const
        {
            SizeType index = FindKey(value);
            if (index == -1)
                return {};
            return m_Data[index].key;
        }

        bool Contains(const KeyType& key) const
        {
            return FindKey(key) != ~0;
        }

        SizeType Count() const { return m_Data.Count(); }

        PairType& GetAt(SizeType index)
        {
            return m_Data.GetAt(index);
        }

        const PairType& GetAt(SizeType index) const
        {
            return m_Data.GetAt(index);
        }

        void Clear()
        {
            m_Data.Clear();
        }

        bool Remove(const PairType& pair)
        {
            if(m_Data.Contains(pair))
            {
                m_Data.Remove(pair);
                return true;
            }
            return false;
        }

        void RemoveAt(const size_t index)
        {
            m_Data.RemoveAt(index);
        }

        bool operator==(const Map& other) const
        {
            return m_Data == other.m_Data;
        }

        bool IsEmpty() const { return m_Data.IsEmpty(); }

        Iterator begin() { return m_Data.begin(); }
        Iterator end()  { return m_Data.end(); }
        ConstIterator begin() const { return m_Data.begin(); }
        ConstIterator end() const { return m_Data.end(); }

    private:
        ArrayType m_Data;
    };
}
