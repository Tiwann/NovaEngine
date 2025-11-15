#pragma once
#include <cstddef>

namespace Nova
{
    template<class T>
    class Iterator
    {
        using SizeType = size_t;
        using ElementType = T;
        using PointerType = ElementType*;
        using ConstPointerType = const ElementType*;
        using RefType = ElementType&;
        using ConstRefType = const ElementType&;
    public:
        Iterator(PointerType Ptr) : m_Data(Ptr) {}

        Iterator operator+(SizeType Count) { return Iteratror(m_Data + Count); }
        
        Iterator& operator++() {  ++m_Data; return *this; }
        Iterator& operator--() {  --m_Data; return *this; }
        
        Iterator operator++(int)
        {
            Iterator It = *this;
            ++(*this);
            return It;
        }
        
        Iterator operator--(int)
        {
            Iterator It = *this;
            --(*this);
            return It;
        }
        
        Iterator& operator+=(SizeType Count) { m_Data += Count; return *this; }
        Iterator& operator-=(SizeType Count) { m_Data -= Count; return *this; }

        PointerType operator->() { return m_Data; }
        ConstPointerType operator->() const { return m_Data; }
        
        RefType operator*() { return *m_Data; }
        ConstRefType operator*() const { return m_Data; }

        RefType operator[](SizeType Index) { return m_Data[Index]; }
        ConstRefType operator[](SizeType Index) const { return m_Data[Index]; }

        bool operator==(const Iterator& Other) const
        {
            return m_Data == Other.m_Data;
        }
    private:
        PointerType m_Data;
    };

    template<class T>
    class ConstIterator
    {
        using SizeType = size_t;
        using ElementType = T;
        using PointerType = ElementType*;
        using ConstPointerType = const ElementType*;
        using RefType = ElementType&;
        using ConstRefType = const ElementType&;
    public:
        ConstIterator(PointerType Ptr) : m_Data(Ptr) {}
        ConstIterator(ConstPointerType Ptr) : m_Data(Ptr) {}

        ConstIterator operator+(SizeType Count) { return ConstIterator(m_Data + Count); }
        ConstIterator operator-(SizeType Count) { return ConstIterator(m_Data - Count); }
        
        ConstIterator& operator++() {  ++m_Data; return *this; }
        ConstIterator& operator--() {  --m_Data; return *this; }
        
        ConstIterator operator++(int)
        {
            ConstIterator It = *this;
            ++(*this);
            return It;
        }
        
        ConstIterator operator--(int)
        {
            ConstIterator It = *this;
            --(*this);
            return It;
        }
        
        ConstIterator& operator+=(SizeType Count) { m_Data += Count; return *this; }
        ConstIterator& operator-=(SizeType Count) { m_Data -= Count; return *this; }
        
        ConstPointerType operator->() const { return m_Data; }
        
        ConstRefType operator*() const { return *m_Data; }
        
        ConstRefType operator[](SizeType Index) const { return m_Data[Index]; }

        bool operator==(const ConstIterator& Other) const
        {
            return m_Data == Other.m_Data;
        }
    
    private:
        ConstPointerType m_Data;
    };


    template<class T>
    struct Iterable
    {
        virtual ~Iterable() = default;
        virtual Iterator<T> begin() = 0;
        virtual Iterator<T> end() = 0;
        virtual ConstIterator<T> begin() const = 0;
        virtual ConstIterator<T> end() const = 0;
    };
}
