#pragma once
#include "Runtime/TypeTraits.h"
#include "String.h"

namespace Nova
{
    template<typename T> requires IsCharacterValue<T>
    class StringViewBase
    {
        using StringType = StringBase<T>;
        using CharacterType = typename StringType::CharacterType;
        using PointerType = typename StringType::PointerType;
        using ConstPointerType = const CharacterType*;
        using StringLiteralType = typename StringType::StringLiteralType;
        using SizeType = typename StringType::SizeType;
        using ConstIterator = typename StringType::ConstIterator;
    public:
        StringViewBase() = default;
        StringViewBase(const StringType& string) : m_Data(string.Data()), m_Count(string.Count()) { }
        StringViewBase(ConstPointerType data) : m_Data(data), m_Count(StringLength(data)){}
        StringViewBase(ConstPointerType data, SizeType count) : m_Data(data), m_Count(count){}
        StringViewBase(decltype(nullptr)) : m_Data(nullptr), m_Count(0){}

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

        bool EndsWith(StringViewBase str)
        {
            return std::string_view(m_Data, m_Count).find_last_of(std::string_view(str.m_Data, str.m_Count)) != std::string_view::npos;
        }

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
