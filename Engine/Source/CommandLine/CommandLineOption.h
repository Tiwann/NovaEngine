#pragma once
#include "Containers/String.h"
#include "Interfaces/Equatable.h"
#include <optional>

namespace Nova
{
    struct CommandLineOptionPossibleValue : IEquatable<CommandLineOptionPossibleValue>
    {
        String Name;
        String HelpText;

        bool Equals(const CommandLineOptionPossibleValue& Other) const override
        {
            return Name == Other.Name && HelpText == Other.HelpText;
        }
    };

    struct CommandLineOptionPossibleValueList
    {
        String Name;
        Array<CommandLineOptionPossibleValue> Values;

        bool operator==(const CommandLineOptionPossibleValueList& Other) const
        {
            return Name == Other.Name && Values == Other.Values;
        }
    };

    struct CommandLineOption
    {
        String::CharacterType ShortName;
        String LongName;
        bool Required;
        bool AllowMultiple;
        String HelpText;
        std::optional<CommandLineOptionPossibleValueList> PossibleValues;

        bool operator==(const CommandLineOption& other) const
        {
            return ShortName == other.ShortName && LongName == other.LongName && Required == other.Required && AllowMultiple == other.AllowMultiple && HelpText == other.HelpText && PossibleValues == other.PossibleValues;
        }
    };
}