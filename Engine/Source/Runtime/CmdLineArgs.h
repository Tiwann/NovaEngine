#pragma once
#include "Runtime/TypeTraits.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include <cstdint>


namespace Nova
{
    template<Character T>
    class CmdLineArgsBase
    {
    public:
        using StringType = StringBase<T>;
        using StringViewType = StringViewBase<T>;
        using SizeType = Array<StringType>::SizeType;

        CmdLineArgsBase() = default;

        explicit CmdLineArgsBase(const int32_t argc, T** argv)
        {
            for (size_t i = 0; i < argc; ++i)
            {
                m_Args.Emplace(StringType(argv[i]));
            }
        }

        Iterator<StringType> begin() { return &m_Args.First(); }
        Iterator<StringType> end() { return &m_Args.Last(); }
        ConstIterator<StringType> begin() const { return &m_Args.First(); }
        ConstIterator<StringType> end() const { return &m_Args.Last(); }

        SizeType Count() const { return m_Args.Count(); }
        StringViewType GetArgument(SizeType index) const
        {
            NOVA_ASSERT(index < m_Args.Count(), "Index was out of bounds!");
            return StringViewType(m_Args[index]);
        }

        bool Any(const Function<bool(StringView)>& predicate) const
        {
            return m_Args.Any(predicate);
        }
    private:
        Array<StringType> m_Args;
    };

    using CmdLineArgs = CmdLineArgsBase<char>;
    using WideCmdLineArgs = CmdLineArgsBase<wchar_t>;
    using CmdLineArgs16 = CmdLineArgsBase<char16_t>;
    using CmdLineArgs32 = CmdLineArgsBase<char32_t>;
    using CmdLineArgs8 = CmdLineArgsBase<char8_t>;
}
