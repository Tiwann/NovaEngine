#pragma once
#include "Array.h"

namespace Nova
{
    template<typename T>
    class ArrayView
    {
        using ArrayType = Array<T>;
        using PointerType = typename ArrayType::PointerType;
        using ConstPointerType = typename ArrayType::ConstPointerType;
        using ConstReferenceType = typename ArrayType::ConstReferenceType;
        using SizeType = typename ArrayType::SizeType;
        using ConstIterator = typename ArrayType::ConstIterator;
        
        using Predicate = typename ArrayType::Predicate;
        
        template<typename Out>
        using ConstSelector = typename ArrayType::template ConstSelector<Out>;
        
    public:
        ArrayView() : m_Data(nullptr), m_Count(0) {}
        explicit ArrayView(const ArrayType& Arr) : m_Data(Arr.Data()), m_Count(Arr.Count()){}

        ArrayView(const ArrayView&) = default;
        ArrayView(ArrayView&&) noexcept = default;
        ArrayView& operator=(const ArrayView&) = default;
        ArrayView& operator=(ArrayView&&) noexcept = default;

        ConstPointerType Data() const { return m_Data; }
        SizeType Count() const { return m_Count; }
        bool IsEmpty() const { return m_Count == 0; }

        ConstIterator begin() const { return m_Data; }
        ConstIterator end() const { return m_Data + m_Count; }
        ConstReferenceType operator[](SizeType Index) const { return m_Data[Index]; }

        bool operator==(const ArrayView& other) const
        {
            return m_Data == other.m_Data && m_Count == other.m_Count;
        }

        Array<ConstPointerType> Where(const Predicate& Predicate) const
        {
            if(!Predicate) return {};
            Array<ConstReferenceType> Result;
            for(SizeType i = 0; i < m_Count; ++i)
            {
                if(Predicate(m_Data[i]))
                    Result.Add(&m_Data[i]);
            }
            return Result;
        }

        ConstPointerType Single(const Predicate& Predicate) const
        {
            if(!Predicate) return nullptr;
            for(SizeType i = 0; i < m_Count; ++i)
            {
                if(Predicate(m_Data[i]))
                    return &m_Data[i];
            }
            return nullptr;
        }

        template<typename Out>
        Array<const Out*> Select(const ConstSelector<Out>& Selector) const
        {
            if(!Selector) return {};
            Array<const Out*> Result;
            for(SizeType i = 0; i < m_Count; ++i)
            {
                Result.Add(Selector(m_Data[i]));
            }
            return Result;
        }
    private:
        ConstPointerType m_Data = nullptr;
        SizeType m_Count = 0;
    };
}
