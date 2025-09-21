#pragma once
#pragma warning(disable:4146)
#include "Runtime/Iterator.h"
#include "Runtime/Memory.h"
#include "Runtime/Assertion.h"
#include "Math/Functions.h"
#include "Function.h"
#include <initializer_list>
#include <algorithm>
#include <cstring>

namespace Nova
{
    template<typename T>
    class Array final : public Iterable<T>
    {
    public:
        using ValueType = T;
        using PointerType = T*;
        using ConstPointerType = const T*;
        using ReferenceType = T&;
        using ConstReferenceType = const T&;
        using SizeType = size_t;
        using ForwardType = T&&;
        using Iterator = Iterator<T>;
        using ConstIterator = ConstIterator<T>;
        using Predicate = Function<bool(ConstReferenceType)>;
        template<typename Out>
        using Selector = Function<Out*(ReferenceType)>;

        template<typename Out>
        using ConstSelector = Function<const Out*(ReferenceType)>;

        static constexpr SizeType InvalidIndex = -1ULL;

        Array()
        {
            m_Allocated = 1;
            m_Data = Memory::Calloc<T>(m_Allocated);
            m_Count = 0;
        }

        Array(ConstReferenceType first)
        {
            m_Allocated = 1;
            m_Data = Memory::Calloc<T>(m_Allocated);
            m_Count = 1;
            m_Data[0] = first;
        }

        Array(const SizeType count)
        {
            m_Allocated = Math::NearestPowerOfTwo<SizeType>(count);
            m_Data = Memory::Calloc<T>(m_Allocated);
            m_Count = count;
        }

        Array(const std::initializer_list<T>& list) : m_Count(list.size()), m_Allocated(list.size())
        {
            m_Data = Memory::Calloc<T>(m_Allocated);
            std::copy(list.begin(), list.end(), m_Data);
        }

        Array(ConstPointerType data, SizeType count) : m_Count(count), m_Allocated(count)
        {
            m_Data = Memory::Calloc<T>(m_Allocated);
            std::copy(data, data + count, m_Data);
        }

        Array(const Array& other) : m_Count(other.m_Count), m_Allocated(other.m_Allocated)
        {
            m_Data = Memory::Calloc<T>(m_Allocated);
            std::copy(other.begin(), other.end(), m_Data);
        }

        Array(Array&& other) noexcept
        {
            m_Data = other.m_Data;
            m_Count = other.m_Count;
            m_Allocated = other.m_Allocated;

            other.m_Data = nullptr;
            other.m_Count = 0;
            other.m_Allocated = 0;
        }

        ~Array() override
        {
            Memory::Free(m_Data);
        }

        Array& operator=(const Array& other)
        {
            if(this == &other)
                return *this;

            m_Allocated = other.m_Allocated;
            m_Count = other.m_Count;
            Memory::Free(m_Data);
            m_Data = Memory::Calloc<T>(m_Allocated);
            std::copy(other.begin(), other.end(), m_Data);
            return *this;
        }

        Array& operator=(Array&& other) noexcept
        {
            if(this == &other)
                return *this;

            m_Data = other.m_Data;
            m_Count = other.m_Count;
            m_Allocated = other.m_Allocated;

            other.m_Data = nullptr;
            other.m_Count = 0;
            other.m_Allocated = 0;
            return *this;
        }

        ReferenceType operator[](SizeType index)
        {
            NOVA_ASSERT(index <= m_Count && m_Count != 0, "Index out of bounds");
            return m_Data[index];
        }

        ConstReferenceType operator[](SizeType index) const
        {
            NOVA_ASSERT(index <= m_Count && m_Count != 0, "Index out of bounds");
            return m_Data[index];
        }

        void SetAt(SizeType index, ConstReferenceType element)
        {
            NOVA_ASSERT(index <= m_Count && m_Count != 0, "Index out of bounds");
            m_Data[index] = element;
        }

        ReferenceType GetAt(SizeType index)
        {
            NOVA_ASSERT(index <= m_Count && m_Count != 0, "Index out of bounds");
            return m_Data[index];
        }

        ConstReferenceType GetAt(SizeType index) const
        {
            NOVA_ASSERT(index <= m_Count && m_Count != 0, "Index out of bounds");
            return m_Data[index];
        }

        ReferenceType First()
        {
            NOVA_ASSERT(m_Count != 0, "Cannot get first element, array is empty!");
            return m_Data[0];
        }

        ConstReferenceType First() const
        {
            NOVA_ASSERT(m_Count != 0, "Cannot get first element, array is empty!");
            return m_Data[0];
        }

        ReferenceType Last()
        {
            NOVA_ASSERT(m_Count != 0, "Cannot get last element, array is empty!");
            return m_Data[m_Count - 1];
        }

        ConstReferenceType Last() const
        {
            NOVA_ASSERT(m_Count != 0, "Cannot get first element, array is empty!");
            return m_Data[m_Count - 1];
        }

        void Add(ConstReferenceType element)
        {
            if(m_Count >= m_Allocated)
            {
                m_Allocated = Realloc(m_Allocated);
                PointerType realloc = Memory::Calloc<T>(m_Allocated);
                for(SizeType i = 0; i < m_Count; ++i)
                    realloc[i] = m_Data[i];
                Memory::Free(m_Data);
                m_Data = realloc;
            }

            m_Data[m_Count++] = element;
        }

        void AddUnique(ConstReferenceType element)
        {
            if (Find(element) == -1)
                Add(element);
        }

        void AddRange(const std::initializer_list<T>& list)
        {
            if(m_Count + list.size() >= m_Allocated)
            {
                do
                {
                    m_Allocated = Realloc(m_Allocated);
                } while (m_Allocated < m_Count + list.size());
                PointerType Realloc = Memory::Calloc<T>(m_Allocated);
                for(SizeType i = 0; i < m_Count; ++i)
                    Realloc[i] = m_Data[i];
                Memory::Free(m_Data);
                m_Data = Realloc;
            }

            std::copy(list.begin(), list.end(), &m_Data[m_Count]);
            m_Count += list.size();
        }

        void AddRange(const Array& other)
        {
            if(m_Count + other.m_Count >= m_Allocated)
            {
                do
                {
                    m_Allocated = Realloc(m_Allocated);
                } while (m_Allocated < m_Count + other.m_Count);

                PointerType Realloc = Memory::Calloc<T>(m_Allocated);
                for(SizeType i = 0; i < m_Count; ++i)
                    Realloc[i] = m_Data[i];
                Memory::Free(m_Data);
                m_Data = Realloc;
            }

            std::copy(other.begin(), other.end(), &m_Data[m_Count]);
            m_Count += other.Count();
        }

        template<size_t N>
        void AddRange(const T(&data)[N])
        {
            if(m_Count + N >= m_Allocated)
            {
                do
                {
                    m_Allocated = Realloc(m_Allocated);
                } while (m_Allocated < m_Count + N);

                PointerType realloc = Memory::Calloc<T>(m_Allocated);
                for(SizeType i = 0; i < m_Count; ++i)
                    realloc[i] = m_Data[i];
                Memory::Free(m_Data);
                m_Data = realloc;
            }

            std::copy(data, data + N, &m_Data[m_Count]);
            m_Count += N;
        }

        void AddRange(ConstPointerType data, SizeType count)
        {
            if(m_Count + count >= m_Allocated)
            {
                do
                {
                    m_Allocated = Realloc(m_Allocated);
                } while (m_Allocated < m_Count + count);
                PointerType Realloc = Memory::Calloc<T>(m_Allocated);
                for(SizeType i = 0; i < m_Count; ++i)
                    Realloc[i] = m_Data[i];
                Memory::Free(m_Data);
                m_Data = Realloc;
            }

            std::copy(data, data + count, &m_Data[m_Count]);
            m_Count += count;
        }

        void Emplace(T&& element)
        {
            if(m_Count >= m_Allocated)
            {
                m_Allocated = Realloc(m_Allocated);
                PointerType Realloc = Memory::Calloc<T>(m_Allocated);
                for(SizeType i = 0; i < m_Count; ++i)
                    Realloc[i] = m_Data[i];
                Memory::Free(m_Data);
                m_Data = Realloc;
            }

            m_Data[m_Count++] = std::move(element);
        }

        Array Union(const Array& other)
        {
            Array Result = *this;
            for (const T& Element : other)
                AddUnique(Element);
            return Result;
        }

        bool Remove(ConstReferenceType Element)
        {
            SizeType Index = Find(Element);
            if(Index == InvalidIndex) return false;

            std::move(m_Data + Index + 1, m_Data + m_Count, m_Data + Index);
            m_Count--;
            return true;
        }

        void RemoveAt(SizeType Index)
        {
            NOVA_ASSERT(Index <= m_Count, "Index out of bounds!");
            std::move(m_Data + Index + 1, m_Data + m_Count, m_Data + Index);
            m_Count--;
        }

        bool RemoveAll(ConstReferenceType Element)
        {
            SizeType Index;
            bool Found = false;
            while ((Index = Find(Element)) != InvalidIndex)
            {
                if(Index < m_Count) std::move(m_Data + Index + 1, m_Data + m_Count, m_Data + Index);
                m_Count--;
                Found = true;
            }
            return Found;
        }

        bool Contains(ConstReferenceType Element) const
        {
            for(SizeType i = 0; i < m_Count; ++i)
                if(m_Data[i] == Element)
                    return true;
            return false;
        }

        SizeType Find(ConstReferenceType Element) const
        {

            for(SizeType i = 0; i < m_Count; ++i)
            {
                if(m_Data[i] == Element)
                {
                    return i;
                }
            }
            return -1;
        }

        void Pop()
        {
            RemoveAt(m_Count);
        }

        void Clear()
        {
            m_Count = 0;
        }

        // Return an array of pointer to elements of type T, inside m_Data, where each element satisfies Predicate
        Array<PointerType> Where(const Predicate& Predicate) const
        {
            if(!Predicate) return {};
            Array<PointerType> Result;
            for(SizeType i = 0; i < m_Count; ++i)
            {
                if(Predicate(m_Data[i]))
                    Result.Add(&m_Data[i]);
            }
            return Result;
        }

        // Return a pointer to first elements of type T, inside m_Data, which satisfy Predicate
        PointerType Single(const Predicate& Predicate) const
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
        Array<Out*> Select(const Selector<Out>& Selector) const
        {
            if(!Selector) return {};
            Array<Out*> Result;
            for(SizeType i = 0; i < m_Count; ++i)
            {
                Result.Add(Selector(m_Data[i]));
            }
            return Result;
        }

        bool All(const Predicate& Predicate) const
        {
            for (SizeType i = 0; i < m_Count; ++i)
            {
                if (!Predicate(m_Data[i]))
                    return false;
            }
            return true;
        }

        bool Any(const Predicate& Predicate) const
        {
            for (SizeType i = 0; i < m_Count; ++i)
            {
                if (Predicate(m_Data[i]))
                    return true;
            }
            return false;
        }

        bool IsEmpty() const { return m_Count == 0; }


        void Sort(const Predicate& Predicate)
        {
            //std::ranges::sort(m_Data, m_Data + m_Count, Predicate);
        }

        template<typename U>
        Array<U> Transform(const Function<U(ConstReferenceType)>& predicate) const
        {
            Array<U> result;
            for (size_t index = 0; index < m_Count; ++index)
            {
                const T& element = m_Data[index];
                result.Add(predicate(element));
            }
            return result;
        }

        Iterator begin() override { return m_Data; }
        Iterator end() override { return m_Data + m_Count; }

        ConstIterator begin() const override{ return m_Data; }
        ConstIterator end() const override { return (PointerType)(m_Data + m_Count); }

        ConstPointerType Data() const { return m_Data; }
        PointerType Data() { return m_Data; }

        SizeType Count() const { return m_Count; }
        size_t Size() const { return m_Count * sizeof(T); }

        bool operator==(const Array& other) const
        {
            if(m_Count != other.m_Count) return false;
            for(SizeType i = 0; i < m_Count; ++i)
                if(m_Data[i] != other.m_Data[i]) return false;
            return true;
        }



    private:
        PointerType m_Data = nullptr;
        SizeType m_Count = 0;
        SizeType m_Allocated = 0;

        static SizeType Realloc(const SizeType Current)
        {
            return Current * 2;
        }
    };
}
