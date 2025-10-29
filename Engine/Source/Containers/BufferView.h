﻿#pragma once
#include "Runtime/Iterator.h"

namespace Nova
{
    template <typename Type>
    class BufferView
    {
    public:
        using Iterator = Iterator<Type>;
        using ConstIterator = ConstIterator<Type>;

        BufferView(const Type* data, size_t count) : m_Data(data), m_Count(count)
        {
        }

        BufferView(const BufferView& buffer) : m_Data(buffer.m_Data), m_Count(buffer.m_Count)
        {
        }

        ConstIterator Begin() const { return m_Data; }
        ConstIterator End() const { return m_Data + m_Count; }

        template <typename OtherType>
        BufferView<OtherType> As() const
        {
            return BufferView<OtherType>((OtherType*)Data(), Count() * (sizeof(Type) / sizeof(OtherType)));
        }

        size_t Count() const
        {
            return m_Count;
        }

        size_t Size() const
        {
            return m_Count * sizeof(Type);
        }

        const Type& operator[](size_t Index) const
        {
            return m_Data[Index];
        }

        const Type* Data() const { return m_Data; }
    private:
        const Type* m_Data = nullptr;
        size_t m_Count = 0;
    };
}
