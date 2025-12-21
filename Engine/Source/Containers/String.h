#pragma once
#include "Containers/Array.h"
#include "Runtime/Iterator.h"
#include "Runtime/Assertion.h"
#include "Runtime/TypeTraits.h"
#include "Containers/BufferView.h"
#include <string_view>
#include <iostream>

namespace Nova
{

    template<typename CharacterType> requires IsCharacterValue<CharacterType>
    static size_t StringLength(const CharacterType* data)
    {
        CharacterType* ptr = const_cast<CharacterType*>(data);
        size_t count = 0;
        while (*ptr != 0)
        {
            count++;
            ++ptr;
        }
        return count;
    }
    
    template<typename T> requires IsCharacterValue<T>
    class StringBase : public Iterable<T>
    {
    public:
        using CharacterType = T;
        using SizeType = size_t;
        using StringLiteralType = const CharacterType*;
        using PointerType = CharacterType*;
        using Iterator = Iterator<T>;
        using ConstIterator = ConstIterator<T>;
        using ArrayType = Array<CharacterType>;
        using BufferType = BufferView<CharacterType>;
        
        static constexpr SizeType CharacterSize = sizeof(CharacterType);
        
        StringBase()
        {
            m_Data = new CharacterType[1]{};
            m_Count = 0;
        }
        
        StringBase(StringLiteralType data)
        {
            NOVA_ASSERT(data, "Cannot construct string with nullptr!");
            m_Count = StringLength(data);
            m_Data = new CharacterType[m_Count + 1]{};
            memcpy(m_Data, data, m_Count * CharacterSize);
        }

        explicit StringBase(const SizeType count) : m_Count(count)
        {
            m_Data = new CharacterType[m_Count + 1]{};
        }

        StringBase(CharacterType* data, SizeType count)
        {
            NOVA_ASSERT(data, "Cannot construct string with nullptr!");
            m_Count = count;
            m_Data = new CharacterType[m_Count + 1]{};
            memcpy(m_Data, data, m_Count * CharacterSize);
        }

        StringBase(const StringBase& other)
        {
            delete[] m_Data;
            m_Data = new CharacterType[other.m_Count + 1]{};
            memcpy(m_Data, other.m_Data, other.m_Count * CharacterSize);
            m_Count = other.m_Count;
        }

        StringBase(StringBase&& Other) noexcept
        {
            m_Data = Other.m_Data;
            m_Count = Other.m_Count;
            Other.m_Data = nullptr;
            Other.m_Count = 0;
        }

        StringBase& operator=(const StringBase& other)
        {
            if(this == &other)
                return *this;

            if(m_Data) delete[] m_Data;
            m_Data = new CharacterType[other.m_Count + 1]{};
            memcpy(m_Data, other.m_Data, other.m_Count * CharacterSize);
            m_Count = other.m_Count;
            return *this;
        }

        StringBase& operator=(StringBase&& other) noexcept
        {
            if(this == &other)
                return *this;

            delete[] m_Data;
            
            m_Data = other.m_Data;
            m_Count = other.m_Count;
            other.m_Data = nullptr;
            other.m_Count = 0;
            return *this;
        }

        template<SizeType N>
        StringBase& operator=(CharacterType (&&buffer)[N])
        {
            const SizeType count = StringLength(buffer);
            if (count < N)
            {
                delete[] m_Data;
                m_Data = new CharacterType[count];
            }

            Memory::Memmove(m_Data, buffer, count * CharacterSize);
            m_Data[m_Count] = 0;
            m_Count = count;
            return *this;
        }

        ~StringBase() override
        {
            delete[] m_Data;
            m_Count = 0;
        }

        CharacterType& operator[](SizeType index)
        {
            NOVA_ASSERT(index <= m_Count, "Index out of bounds");
            return m_Data[index];
        }
        
        const CharacterType& operator[](SizeType index) const
        {
            NOVA_ASSERT(index <= m_Count, "Index out of bounds");
            return m_Data[index];
        }

        bool operator==(const StringBase& other) const
        {
            return std::strcmp(m_Data, other.m_Data) == 0 && m_Count == other.m_Count;
        }

        bool IsEmpty() const { return m_Count == 0; }

        CharacterType* Data() { return m_Data; }
        const CharacterType* Data() const { return m_Data; }
        
        CharacterType* operator*() { return m_Data; }
        const CharacterType* operator*() const { return m_Data; }
        
        SizeType Count() const { return m_Count; }
        SizeType Size() const { return m_Count * CharacterSize; }


        StringBase& Resize(const SizeType newCount)
        {
            if (m_Count == newCount) return *this;

            if (m_Count > newCount)
            {
                CharacterType* newData = new CharacterType [newCount + 1]{0};
                Memory::Memcpy(newData, m_Data, m_Count * CharacterSize);
                delete [] m_Data;
                m_Data = newData;
                m_Count = newCount;
                return *this;
            }

            if (m_Count < newCount)
            {
                CharacterType* newData = new CharacterType [newCount + 1]{0};
                Memory::Memcpy(newData, m_Data, newCount * CharacterSize);
                delete [] m_Data;
                m_Data = newData;
                m_Count = newCount;
                return *this;
            }

            return *this;
        }
        
        StringBase& Append(StringLiteralType data)
        {
            NOVA_ASSERT(data, "Cannot append string with nullptr string literal!");
            const SizeType dataCount = StringLength(data);
            const SizeType newCount = m_Count + dataCount;
            CharacterType* newData = new CharacterType[newCount + 1]{};
            memcpy(newData, m_Data, m_Count * CharacterSize);
            memcpy(newData + m_Count, data, dataCount * CharacterSize);
            delete [] m_Data;
            m_Data = newData;
            m_Count = newCount;
            return *this;
        }

        
        StringBase& Append(CharacterType character)
        {
            const SizeType newCount = m_Count + 1;
            CharacterType* newData = new CharacterType[newCount + 1]{};
            memcpy(newData, m_Data, m_Count * CharacterSize);
            memcpy(newData + m_Count, &character, CharacterSize);
            delete [] m_Data;
            m_Data = newData;
            m_Count = newCount;
            return *this;
        }

        StringBase& Append(const StringBase& string)
        {
            const SizeType dataCount = string.Count();
            const SizeType newCount = m_Count + dataCount;
            CharacterType* newData = new CharacterType[newCount + 1]{};
            memcpy(newData, m_Data, m_Count * CharacterSize);
            memcpy(newData + m_Count, string.Data(), dataCount * CharacterSize);
            delete [] m_Data;
            m_Data = newData;
            m_Count = newCount;
            return *this;
        }
        
        StringBase Substring(SizeType begin, SizeType end) const
        {
            NOVA_ASSERT(begin < m_Count && begin + (end - begin) <= m_Count, "Indices out of bounds!");
            const SizeType newCount = end - begin + 1;
            CharacterType* newData = new CharacterType[newCount + 1]{};
            memcpy(newData, m_Data + begin, newCount * CharacterSize);
            return {newData, newCount};
        }

        StringBase Substring(const SizeType begin) const
        {
            return Substring(begin, m_Count);
        }
        
        SizeType Find(CharacterType character) const
        {
            for(SizeType i = 0; i < m_Count; ++i)
            {
                if(m_Data[i] == character)
                    return i;
            }
            return -1;
        }

        SizeType Find(const StringBase& string) const
        {
            std::basic_string_view<CharacterType> view(m_Data, m_Count);
            std::basic_string_view<CharacterType> otherView(string.m_Data, string.m_Count);
            return view.find(otherView);
        }

        SizeType Find(SizeType index, const StringBase& string) const
        {
            std::basic_string_view<CharacterType> view(m_Data + index, m_Count);
            std::basic_string_view<CharacterType> otherView(string.m_Data, string.m_Count);
            return view.find(otherView);
        }

        SizeType FindLast(const StringBase& string) const
        {
            std::basic_string_view<CharacterType> view(m_Data, m_Count);
            std::basic_string_view<CharacterType> otherView(string.m_Data, string.m_Count);
            return view.find_last_of(otherView);
        }

        bool EndsWith(const StringBase& string) const
        {
            return FindLast(string) != SizeType(-1);
        }

        bool StartsWith(const StringBase& string) const
        {
            return Find(string) == 0;
        }
        
        SizeType OccurrencesOf(CharacterType character) const
        {
            SizeType result = 0;
            for(SizeType i = 0; i < m_Count; ++i)
            {
                CharacterType current = m_Data[i];
                if(current == character)
                    result++;
            }
            return result;
        }

        StringBase& Replace(const StringBase& from, const StringBase& to)
        {
            const SizeType index = Find(from);
            if(index == -1ULL) return *this;

            if (to.Count() <= from.Count())
            {
                memcpy(m_Data + index, *to, to.Size());
                return *this;
            }
            
            const SizeType delta = to.Count() - from.Count();
            const SizeType newCount = m_Count + delta;
            CharacterType* newData = new CharacterType[newCount]{};
                
            CharacterType* dest = newData;
            CharacterType* src = m_Data;
            SizeType size = index * CharacterSize;
            memcpy(dest, src, size);
                
            dest = newData + index * CharacterSize;
            src = const_cast<CharacterType*>(*to);
            size = to.Size();
            memcpy(dest, src, size);

            dest = newData + index * CharacterSize + to.Size();
            src = m_Data + index * CharacterSize + from.Size();
            size = m_Count * CharacterSize - (index * CharacterSize + from.Size());
            memcpy(dest, src, size);

            delete [] m_Data;
            m_Data = newData;
            m_Count = newCount;
            return *this;
        }

        StringBase& Replace(SizeType index, SizeType count, const StringBase& to)
        {
            NOVA_ASSERT(index < m_Count && index + count <= m_Count, "Range is out of bounds!");
            if (index == -1ULL) return *this;

            if (to.Count() <= count)
            {
                memcpy(m_Data + index, *to, to.Size());
                return *this;
            }

            const SizeType delta = to.Count() - count;
            const SizeType newCount = m_Count + delta;
            CharacterType* newData = new CharacterType[newCount]{};
                
            CharacterType* dest = newData;
            CharacterType* src = m_Data;
            SizeType size = index * CharacterSize;
            memcpy(dest, src, size);
                
            dest = newData + index * CharacterSize;
            src = const_cast<CharacterType*>(*to);
            size = to.Size();
            memcpy(dest, src, size);

            dest = newData + index * CharacterSize + to.Size();
            src = m_Data + index * CharacterSize + count * CharacterSize;
            size = m_Count * CharacterSize - (index * CharacterSize + count * CharacterSize);
            memcpy(dest, src, size);

            delete [] m_Data;
            m_Data = newData;
            m_Count = newCount;
            return *this;
        }

        StringBase& ReplaceAll(const StringBase& from, const StringBase& to)
        {
            SizeType index = 0;
            while ((index = Find(index, from)) != -1ULL)
            {
                Replace(index, m_Count - index, to);
            }
            return *this;
        }

        StringBase& ReplaceAll(CharacterType from, CharacterType to)
        {
            if (from == to) return *this;
            if (!m_Data || m_Count <= 0) return *this;

            CharacterType* ptr = m_Data;
            while (ptr < m_Data + m_Count)
            {
                if (*ptr == from)
                    *ptr = to;
                ++ptr;
            }
            return *this;
        }

        StringBase& Remove(SizeType from, SizeType to)
        {
            NOVA_ASSERT(from < to, "Range is illegal");
            NOVA_ASSERT(from < m_Count, "Range is illegal");
            NOVA_ASSERT(to <= m_Count, "Range is illegal");
            NOVA_ASSERT(from + (to - from) <= m_Count, "Range is illegal");
            const SizeType delta = to - from;
            
            const CharacterType* src = m_Data + to;
            CharacterType* dest = m_Data + from;
            const SizeType size = (m_Count - to) * CharacterSize;
            ::memmove(dest, src, size);
            m_Count -= delta;
            m_Data[m_Count] = 0;
            return *this;
        }

        StringBase& Remove(const StringBase& string)
        {
            const SizeType index = Find(string);
            if (index == -1ULL) return *this;
            Remove(index, index + string.Count());
            return *this;
        }
        
        Iterator begin() override { return m_Data; }
        Iterator end() override { return m_Data + m_Count; }
        ConstIterator begin() const override { return m_Data; }
        ConstIterator end() const override { return m_Data + m_Count; }

        ArrayType AsArray() const { return ArrayType(m_Data, m_Count); }
        BufferType AsBuffer() const { return BufferType(m_Data, m_Count); }

        template<typename U> requires (sizeof(T) % sizeof(U) == 0)
        BufferView<U> GetView() { return BufferView<U>((U*)m_Data, m_Count); }
        
        StringBase& operator+(const StringBase& other)
        {
            return Append(other);
        }

        StringBase& TrimEnd(CharacterType character)
        {
            SizeType count = 0;
            for (size_t index = m_Count - 1; index > 0; --index)
            {
                if (m_Data[index] != character)
                    break;
                count++;
            }

            if (count == 0)
                return *this;

            m_Count -= count;
            m_Data[m_Count] = 0;
            return *this;
        }

        StringBase TrimStart(CharacterType character) const
        {
            StringBase copy(*this);
            SizeType count = 0;
            while (count < copy.m_Count && copy.m_Data[count] == character)
                ++count;

            if (count == 0)
                return copy;

            memmove(copy.m_Data, copy.m_Data + count, (copy.m_Count - count) * CharacterSize);
            copy.m_Count -= count;
            copy.m_Data[m_Count] = 0;
            return copy;
        }

        StringBase TrimStart(const Array<CharacterType>& characters) const
        {
            StringBase copy(*this);
            SizeType count = 0;
            while (count < copy.m_Count && characters.Contains(copy.m_Data[count]))
                ++count;

            if (count == 0)
                return *this;

            ::memmove(copy.m_Data, copy.m_Data + count, (copy.m_Count - count) * CharacterSize);
            copy.m_Count -= count;
            copy.m_Data[copy.m_Count] = 0;
            return copy;
        }

        friend std::basic_ostream<CharacterType>& operator<<(std::basic_ostream<CharacterType>& os, const StringBase& string)
        {
            os.write(string.m_Data, string.m_Count * CharacterSize);
            os.flush();
            return os;
        }

    private:
        CharacterType* m_Data = nullptr;
        size_t m_Count = 0;
    };

    using String = StringBase<char>;
    using String8 = StringBase<char>;
    using String16 = StringBase<char16_t>;
    using String32 = StringBase<char32_t>;
    using WideString = StringBase<wchar_t>;
}






