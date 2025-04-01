#pragma once
#include "Array.h"

namespace Nova
{
    template <typename T>
    class Queue
    {
    public:
        using ArrayType = Array<T>;
        using SizeType = typename ArrayType::SizeType;
        using ValueType = typename ArrayType::ValueType;
        using Iterator = typename ArrayType::Iterator;
        using ConstIterator = typename ArrayType::ConstIterator;
        using ReferenceType = typename ArrayType::ReferenceType;
        using ConstReferenceType = typename ArrayType::ConstReferenceType;

        Queue() = default;
        Queue(const Queue&) = default;
        Queue(Queue&&) = default;
        Queue& operator=(const Queue&) = default;
        Queue& operator=(Queue&&) = default;
        
        void Enqueue(ConstReferenceType Item)
        {
            m_Data.Add(Item);
        }

        ValueType Dequeue()
        {
            ValueType First = m_Data.First();
            m_Data.RemoveAt(0);
            return First;
        }

        bool IsEmpty() const { return m_Data.IsEmpty(); }

        
    private:
        ArrayType m_Data;
    };
}