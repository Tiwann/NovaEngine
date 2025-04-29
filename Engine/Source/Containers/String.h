#pragma once
#include "Containers/Array.h"
#include "Runtime/Iterator.h"
#include "Runtime/Assertion.h"
#include "Runtime/TypeTraits.h"
#include "Containers/Buffer.h"
#include <string_view>
#include <iostream>

namespace Nova
{

    template<typename CharacterType> requires IsCharacterValue<CharacterType>
    static size_t StringLength(const CharacterType* Data)
    {
        CharacterType* Ptr = const_cast<CharacterType*>(Data);
        size_t Count = 0;
        while (*Ptr != 0)
        {
            Count++;
            ++Ptr;
        }
        return Count;
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
        using BufferType = Buffer<CharacterType>;
        
        static inline const SizeType CharacterSize = sizeof(CharacterType);
        
        StringBase()
        {
            m_Data = new CharacterType[1]{};
            m_Count = 0;
        }
        
        StringBase(StringLiteralType Data)
        {
            NOVA_ASSERT(Data, "Cannot construct string with nullptr!");
            m_Count = StringLength(Data);
            m_Data = new CharacterType[m_Count + 1]{};
            memcpy(m_Data, Data, m_Count * CharacterSize);
        }

        StringBase(CharacterType* Data, SizeType Count)
        {
            NOVA_ASSERT(Data, "Cannot construct string with nullptr!");
            m_Count = Count;
            m_Data = new CharacterType[m_Count + 1]{};
            memcpy(m_Data, Data, m_Count * CharacterSize);
        }
        
        StringBase(const StringBase& Other)
        {
            delete m_Data;
            m_Data = new CharacterType[Other.m_Count + 1]{};
            memcpy(m_Data, Other.m_Data, Other.m_Count * CharacterSize);
            m_Count = Other.m_Count;
        }

        StringBase(StringBase&& Other) noexcept
        {
            m_Data = Other.m_Data;
            m_Count = Other.m_Count;
            Other.m_Data = nullptr;
            Other.m_Count = 0;
        }

        StringBase& operator=(const StringBase& Other)
        {
            if(this == &Other)
                return *this;

            if(m_Data) delete[] m_Data;
            m_Data = new CharacterType[Other.m_Count + 1]{};
            memcpy(m_Data, Other.m_Data, Other.m_Count * CharacterSize);
            m_Count = Other.m_Count;
            return *this;
        }

        StringBase& operator=(StringBase&& Other) noexcept
        {
            if(this == &Other)
                return *this;

            delete[] m_Data;
            
            m_Data = Other.m_Data;
            m_Count = Other.m_Count;
            Other.m_Data = nullptr;
            Other.m_Count = 0;
            return *this;
        }

        ~StringBase() override
        {
            delete[] m_Data;
            m_Count = 0;
        }

        CharacterType& operator[](SizeType Index)
        {
            NOVA_ASSERT(Index <= m_Count, "Index out of bounds");
            return m_Data[Index];
        }
        
        const CharacterType& operator[](SizeType Index) const
        {
            NOVA_ASSERT(Index <= m_Count, "Index out of bounds");
            return m_Data[Index];
        }

        bool operator==(const StringBase& Other) const
        {
            return std::strcmp(m_Data, Other.m_Data) == 0 && m_Count == Other.m_Count;
        }

        bool IsEmpty() const { return m_Count == 0; }

        CharacterType* Data() { return m_Data; }
        const CharacterType* Data() const { return m_Data; }
        
        CharacterType* operator*() { return m_Data; }
        const CharacterType* operator*() const { return m_Data; }
        
        SizeType Count() const { return m_Count; }
        SizeType Size() const { return m_Count * CharacterSize; }
        
        StringBase& Append(StringLiteralType Data)
        {
            NOVA_ASSERT(Data, "Cannot append string with nullptr string literal!");
            const SizeType DataCount = StringLength(Data);
            const SizeType NewCount = m_Count + DataCount;
            CharacterType* NewData = new CharacterType[NewCount + 1]{};
            memcpy(NewData, m_Data, m_Count * CharacterSize);
            memcpy(NewData + m_Count, Data, DataCount * CharacterSize);
            delete [] m_Data;
            m_Data = NewData;
            m_Count = NewCount;
            return *this;
        }

        
        StringBase& Append(CharacterType Character)
        {
            const SizeType NewCount = m_Count + 1;
            CharacterType* NewData = new CharacterType[NewCount + 1]{};
            memcpy(NewData, m_Data, m_Count * CharacterSize);
            memcpy(NewData + m_Count, &Character, CharacterSize);
            delete [] m_Data;
            m_Data = NewData;
            m_Count = NewCount;
            return *this;
        }

        StringBase& Append(const StringBase& String)
        {
            const SizeType DataCount = String.Count();
            const SizeType NewCount = m_Count + DataCount;
            CharacterType* NewData = new CharacterType[NewCount + 1]{};
            memcpy(NewData, m_Data, m_Count * CharacterSize);
            memcpy(NewData + m_Count, String.Data(), DataCount * CharacterSize);
            delete [] m_Data;
            m_Data = NewData;
            m_Count = NewCount;
            return *this;
        }
        
        StringBase Substring(SizeType Begin, SizeType End) const
        {
            NOVA_ASSERT(Begin < m_Count && Begin + (End - Begin) <= m_Count, "Indices out of bounds!");
            const SizeType NewCount = End - Begin + 1;
            CharacterType* NewData = new CharacterType[NewCount + 1]{};
            memcpy(NewData, m_Data + Begin, NewCount * CharacterSize);
            return {NewData, NewCount};
        }

        StringBase Substring(const SizeType Begin) const
        {
            return Substring(Begin, m_Count);
        }
        
        SizeType Find(CharacterType Char) const
        {
            for(SizeType i = 0; i < m_Count; ++i)
            {
                if(m_Data[i] == Char)
                    return i;
            }
            return -1;
        }

        SizeType Find(const StringBase& Str) const
        {
            std::basic_string_view<CharacterType> View(m_Data, m_Count);
            std::basic_string_view<CharacterType> Other(Str.m_Data, Str.m_Count);
            return View.find(Other);
        }

        SizeType Find(SizeType Index, const StringBase& Str) const
        {
            std::basic_string_view<CharacterType> View(m_Data + Index, m_Count);
            std::basic_string_view<CharacterType> Other(Str.m_Data, Str.m_Count);
            return View.find(Other);
        }
        
        SizeType OccurrencesOf(CharacterType Char)
        {
            SizeType Result = 0;
            for(SizeType i = 0; i < m_Count; ++i)
            {
                CharacterType Curr = m_Data[i];
                if(Curr == Char)
                    Result++;
            }
            return Result; 
        }

        bool StartsWith(const StringBase& Str)
        {
            const SizeType Found = Find(Str);
            return Found == 0;
        }

        template<typename... Args>
        u32 ScanFormat(const StringBase& Format, Args*... Arguments)
        {
            return std::sscanf(m_Data, Format.m_Data, Arguments...);
        }

        StringBase& Replace(const StringBase& From, const StringBase& To)
        {
            const SizeType Index = Find(From);
            if(Index == -1ULL) return *this;

            if (To.Count() <= From.Count())
            {
                memcpy(m_Data + Index, *To, To.Size());
                return *this;
            }
            
            const SizeType Delta = To.Count() - From.Count();
            const SizeType NewCount = m_Count + Delta;
            CharacterType* NewData = new CharacterType[NewCount]{};
                
            CharacterType* Dest = NewData;
            CharacterType* Src = m_Data;
            SizeType Size = Index * CharacterSize;
            memcpy(Dest, Src, Size);
                
            Dest = NewData + Index * CharacterSize;
            Src = const_cast<CharacterType*>(*To);
            Size = To.Size();
            memcpy(Dest, Src, Size);

            Dest = NewData + Index * CharacterSize + To.Size();
            Src = m_Data + Index * CharacterSize + From.Size();
            Size = m_Count * CharacterSize - (Index * CharacterSize + From.Size());
            memcpy(Dest, Src, Size);

            delete [] m_Data;
            m_Data = NewData;
            m_Count = NewCount;
            return *this;
        }

        StringBase& Replace(SizeType Index, SizeType Count, const StringBase& To)
        {
            NOVA_ASSERT(Index < m_Count && Index + Count <= m_Count, "Range is out of bounds!");
            if (Index == -1ULL) return *this;

            if (To.Count() <= Count)
            {
                memcpy(m_Data + Index, *To, To.Size());
                return *this;
            }

            const SizeType Delta = To.Count() - Count;
            const SizeType NewCount = m_Count + Delta;
            CharacterType* NewData = new CharacterType[NewCount]{};
                
            CharacterType* Dest = NewData;
            CharacterType* Src = m_Data;
            SizeType Size = Index * CharacterSize;
            memcpy(Dest, Src, Size);
                
            Dest = NewData + Index * CharacterSize;
            Src = const_cast<CharacterType*>(*To);
            Size = To.Size();
            memcpy(Dest, Src, Size);

            Dest = NewData + Index * CharacterSize + To.Size();
            Src = m_Data + Index * CharacterSize + Count * CharacterSize;
            Size = m_Count * CharacterSize - (Index * CharacterSize + Count * CharacterSize);
            memcpy(Dest, Src, Size);

            delete [] m_Data;
            m_Data = NewData;
            m_Count = NewCount;
            return *this;
        }

        StringBase& ReplaceAll(const StringBase& From, const StringBase& To)
        {
            SizeType Index = 0;
            while ((Index = Find(Index, From)) != -1ULL)
            {
                Replace(Index, m_Count - Index, To);
            }
            return *this;
        }

        StringBase& Remove(SizeType From, SizeType To)
        {
            NOVA_ASSERT(From < To, "Range is illegal");
            NOVA_ASSERT(From < m_Count, "Range is illegal");
            NOVA_ASSERT(To <= m_Count, "Range is illegal");
            NOVA_ASSERT(From + (To - From) <= m_Count, "Range is illegal");
            const SizeType Delta = To - From;
            
            const CharacterType* Src = m_Data + To;
            CharacterType* Dest = m_Data + From;
            const SizeType Size = (m_Count - To) * CharacterSize;
            ::memmove(Dest, Src, Size);
            m_Count -= Delta;
            m_Data[m_Count] = 0;
            return *this;
        }

        StringBase& Remove(const StringBase& Str)
        {
            const SizeType Index = Find(Str);
            if (Index == -1ULL) return *this;
            Remove(Index, Index + Str.Count());
            return *this;
        }
        
        Iterator begin() override { return m_Data; }
        Iterator end() override { return m_Data + m_Count; }
        ConstIterator begin() const override { return m_Data; }
        ConstIterator end() const override { return m_Data + m_Count; }

        ArrayType AsArray() const { return ArrayType(m_Data, m_Count); }
        BufferType AsBuffer() const { return BufferType(m_Data, m_Count); }

        template<typename U>
        BufferView<U> GetView() { return BufferView<U>((U*)m_Data, m_Count); }
        
        StringBase& operator+(const StringBase& Other)
        {
            return Append(Other);
        }

        StringBase& TrimEnd(CharacterType Character)
        {
            SizeType Count = 0;
            for (size_t Index = m_Count - 1; Index > 0; --Index)
            {
                if (m_Data[Index] != Character)
                    break;
                Count++;
            }

            if (Count == 0)
                return *this;

            m_Count -= Count;
            m_Data[m_Count] = 0;
            return *this;
        }

        StringBase TrimStart(CharacterType Character) const
        {
            StringBase Copy(*this);
            SizeType Count = 0;
            while (Count < Copy.m_Count && Copy.m_Data[Count] == Character)
                ++Count;

            if (Count == 0)
                return Copy;

            memmove(Copy.m_Data, Copy.m_Data + Count, (Copy.m_Count - Count) * CharacterSize);
            Copy.m_Count -= Count;
            Copy.m_Data[m_Count] = 0;
            return Copy;
        }

        StringBase TrimStart(const Array<CharacterType>& Characters) const
        {
            StringBase Copy(*this);
            SizeType Count = 0;
            while (Count < Copy.m_Count && Characters.Contains(Copy.m_Data[Count]))
                ++Count;

            if (Count == 0)
                return *this;

            ::memmove(Copy.m_Data, Copy.m_Data + Count, (Copy.m_Count - Count) * CharacterSize);
            Copy.m_Count -= Count;
            Copy.m_Data[m_Count] = 0;
            return Copy;
        }

        ArrayType ToArray() const
        {
            return ArrayType(m_Data, m_Count);
        }


        friend std::basic_ostream<CharacterType>& operator<<(std::basic_ostream<CharacterType>& os, const StringBase& Str)
        {
            os.write(Str.m_Data, Str.m_Count * CharacterSize);
            os.flush();
            return os;
        }

    private:
        CharacterType* m_Data = nullptr;
        size_t m_Count = 0;
    };

#if defined(NOVA_WIDE_STRINGS_BY_DEFAULT)
    using String = StringBase<wchar_t>;
#else
    using String = StringBase<char>;
#endif
    using String8 = StringBase<char>;
    using String16 = StringBase<char16_t>;
    using String32 = StringBase<char32_t>;
    using WideString = StringBase<wchar_t>;
}






