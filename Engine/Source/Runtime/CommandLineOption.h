#pragma once
#include "Containers/String.h"
#include <optional>

namespace Nova
{
    struct CommandLineOptionPossibleValue
    {
        String name;
        String helpText;

        bool operator==(const CommandLineOptionPossibleValue& other) const
        {
            return name == other.name && helpText == other.helpText;
        }
    };

    struct CommandLineOptionPossibleValueList
    {
        String name;
        Array<CommandLineOptionPossibleValue> values;

        bool operator==(const CommandLineOptionPossibleValueList& other) const
        {
            return name == other.name && values == other.values;
        }
    };

    struct CommandLineOption
    {
        String::CharacterType shortName;
        String longName;
        bool required;
        bool allowMultiple;
        String helpText;
        std::optional<CommandLineOptionPossibleValueList> possibleValues;

        bool operator==(const CommandLineOption& other) const
        {
            return shortName == other.shortName && longName == other.longName && required == other.required &&
                allowMultiple == other.allowMultiple && helpText == other.helpText && possibleValues == other.
                possibleValues;
        }
    };
}