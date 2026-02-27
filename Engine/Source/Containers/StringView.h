#pragma once
#include "Runtime/TypeTraits.h"
#include "String.h"

namespace Nova
{
    template<typename T> requires IsCharacterValue<T>
    class StringViewBase
    {
        using StringType = StringBase<T>;
        using CharacterType = StringType::CharacterType;
        using PointerType = StringType::PointerType;
        using ConstPointerType = const CharacterType*;
        using StringLiteralType = StringType::StringLiteralType;
        using SizeType = StringType::SizeType;
        using ConstIterator = StringType::ConstIterator;
    public:
        StringViewBase() = default;
        StringViewBase(const StringType& string) : m_Data(string.Data()), m_Count(string.Count()) { }
        StringViewBase(ConstPointerType data) : m_Data(data), m_Count(StringLength(data)){}
        constexpr StringViewBase(ConstPointerType data, SizeType count) : m_Data(data), m_Count(count){}
        constexpr StringViewBase(decltype(nullptr)) : m_Data(nullptr), m_Count(0){}

        StringViewBase(const StringViewBase&) = default;
        StringViewBase(StringViewBase&&) noexcept = default;
        StringViewBase& operator=(const StringViewBase&) = default;
        StringViewBase& operator=(StringViewBase&&) noexcept = default;

        bool operator==(const StringViewBase& Other) const
        {
            return m_Count == Other.m_Count && Memory::Memcmp(m_Data, Other.m_Data, m_Count * StringType::CharacterSize);
        }

        bool IsValid() const { return m_Data && m_Count != 0; }
        ConstPointerType Data() const { return m_Data; }
        SizeType Count() const { return m_Count; }
        SizeType Size() const { return m_Count * sizeof(CharacterType); }

        ConstIterator begin() const { return m_Data; }
        ConstIterator end() const { return m_Data + m_Count; }

        bool IsEmpty() const { return m_Data == nullptr || m_Count == 0; }

        operator const CharacterType*() const { return m_Data; }
        const CharacterType* operator*() const { return m_Data; }

        const CharacterType& operator[](SizeType index) const
        {
            NOVA_ASSERT(index < m_Count, "Index out of bounds!");
            return m_Data[index];
        }

        SizeType Find(StringType::CharacterType character)
        {
            std::basic_string_view<CharacterType> view(m_Data, m_Count);
            return view.find(character);
        }

        SizeType Find(const StringViewBase& string) const
        {
            std::basic_string_view<CharacterType> view(m_Data, m_Count);
            std::basic_string_view<CharacterType> otherView(string.m_Data, string.m_Count);
            return view.find(otherView);
        }

        SizeType Find(SizeType index, const StringViewBase& str) const
        {
            std::basic_string_view<CharacterType> view(m_Data + index, m_Count);
            std::basic_string_view<CharacterType> otherView(str.m_Data, str.m_Count);
            return view.find(otherView);
        }

        SizeType FindLast(const StringViewBase& string) const
        {
            std::basic_string_view<CharacterType> view(m_Data, m_Count);
            std::basic_string_view<CharacterType> otherView(string.m_Data, string.m_Count);
            return view.find_last_of(otherView);
        }

        bool StartsWith(const StringViewBase& string) const
        {
            return Find(string) == 0;
        }

        bool EndsWith(const StringViewBase& string) const
        {
            return FindLast(string) != SizeType(-1);
        }

        friend std::basic_ostream<CharacterType>& operator<<(std::basic_ostream<CharacterType>& os, const StringViewBase& string)
        {
            os.write(string.m_Data, string.m_Count);
            os.flush();
            return os;
        }
    private:
        ConstPointerType m_Data = nullptr;
        SizeType m_Count = 0;
    };

    using StringView = StringViewBase<char>;
    using StringView16 = StringViewBase<char16_t>;
    using StringView32 = StringViewBase<char32_t>;
    using WideStringView = StringViewBase<wchar_t>;
}
