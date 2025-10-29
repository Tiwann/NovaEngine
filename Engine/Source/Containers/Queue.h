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
        using ReferenceType = typename ArrayType::ReferenceType;
        using ConstReferenceType = typename ArrayType::ConstReferenceType;
        using ForwardType = typename ArrayType::ForwardType;
        using Iterator = typename ArrayType::Iterator;
        using ConstIterator = typename ArrayType::ConstIterator;

        Queue() = default;
        Queue(const Queue&) = default;
        Queue(Queue&&) = default;
        Queue& operator=(const Queue&) = default;
        Queue& operator=(Queue&&) = default;
        
        void Enqueue(ConstReferenceType item)
        {
            m_Data.Add(item);
        }

        template<typename... Args>
        void Enqueue(Args&&... args)
        {
            m_Data.Emplace(args);
        }

        ValueType Dequeue()
        {
            ValueType first = m_Data.First();
            m_Data.PopHead();
            return first;
        }

        bool IsEmpty() const { return m_Data.IsEmpty(); }

        
    private:
        ArrayType m_Data;
    };
}