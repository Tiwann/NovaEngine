#pragma once
#include "Runtime/Iterator.h"
#include "Runtime/Assertion.h"
#include "Containers/Function.h"
#include "Array.h"

#include <initializer_list>
#include <algorithm>


namespace Nova
{
    template<typename T, size_t N>
    class StaticArray : public Iterable<T>
    {
    public:
        using ValueType = T;
        using PointerType = T*;
        using ConstPointerType = const T*;
        using ReferenceType = T&;
        using ConstReferenceType = const T&;
        using Iterator = Iterator<T>;
        using ConstIterator = ConstIterator<T>;
        using SizeType = size_t;
        using Predicate = Function<bool(ConstReferenceType)>;
        template<typename Out>
        using Selector = Function<Out*(ReferenceType)>;
        
        StaticArray()
        {
            //Fill(T());
        }

        constexpr StaticArray(const std::initializer_list<T>& List, SizeType Number = N)
        {
            std::copy(List.begin(), List.end(), m_Data);
        }
        
        StaticArray(ConstPointerType Data, SizeType Count)
        {
            NOVA_ASSERT(Count <= N, "Cannot fill in an Array<N> with more elements than N");
            std::copy(Data, Data + Count, m_Data);
        }

        StaticArray(const StaticArray& Other)
        {
            std::copy(Other.begin(), Other.end(), m_Data);
        }
        
        StaticArray& operator=(const StaticArray& Other)
        {
            if(this == &Other)
                return *this;
            
            std::memcpy(m_Data, Other.m_Data, N * sizeof(T));
            return *this;
        }
        

        ReferenceType operator[](SizeType Index)
        {
            NOVA_ASSERT(Index < N, "Index out of bounds");
            return m_Data[Index];
        }

        ConstReferenceType operator[](SizeType Index) const
        {
            NOVA_ASSERT(Index < N, "Index out of bounds");
            return m_Data[Index];
        }

        PointerType operator*()
        {
            return m_Data;
        }

        ConstPointerType operator*() const
        {
            return m_Data;
        }
        
        void SetAt(SizeType Index, ConstReferenceType Element)
        {
            NOVA_ASSERT(Index < N, "Index out of bounds");
            m_Data[Index] = Element;
        }

        ReferenceType GetAt(SizeType Index)
        {
            NOVA_ASSERT(Index < N, "Index out of bounds");
            return m_Data[Index];
        }

        ConstReferenceType GetAt(SizeType Index) const
        {
            NOVA_ASSERT(Index < N, "Index out of bounds");
            return m_Data[Index];
        }

        
        bool Contains(ConstReferenceType Element)
        {
            for(SizeType i = 0; i < N; ++i)
                if(m_Data[i] == Element)
                    return true;
            return false;
        }

        SizeType Find(ConstReferenceType Element)
        {
            for(SizeType i = 0; i < N; ++i)
                if(m_Data[i] == Element)
                    return i;
            return -1;
        }

        void Fill(const T& Value)
        {
            for(SizeType i = 0; i < N; ++i)
                m_Data[i] = Value;
        }
        
        Array<PointerType> Where(const Predicate& Predicate) const
        {
            if(!Predicate) return {};
            Array<PointerType> Result;
            for(SizeType i = 0; i < N; ++i)
            {
                if(Predicate(m_Data[i]))
                    Result.Add(&m_Data[i]);
            }
            return Result;
        }

        PointerType Single(const Predicate& Predicate) const
        {
            if(!Predicate) return nullptr;
            for(SizeType i = 0; i < N; ++i)
            {
                if(Predicate(m_Data[i]))
                    return &m_Data[i];
            }
            return nullptr;
        }

        template<typename Out>
        Array<Out*> Select(const Selector<Out>& Selector) const
        {
            if(!Selector) return {};
            Array<Out*> Result;
            for(SizeType i = 0; i < N; ++i)
            {
                Result.Add(Selector(m_Data[i]));
            }
            return Result;
        }
        
        Iterator begin() override { return m_Data; }
        Iterator end() override { return m_Data + N; }
        
        ConstIterator begin() const override{ return m_Data; }
        ConstIterator end() const override { return m_Data + N; }

        ConstPointerType Data() const { return m_Data; }
        PointerType Data() { return m_Data; }

        constexpr SizeType Count() const { return N; }
        constexpr SizeType Size() const { return N * sizeof(T); }

        operator PointerType() { return m_Data; }
        operator ConstPointerType() const { return m_Data; }
    private:
        ValueType m_Data[N];
    };
}
