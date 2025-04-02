#pragma once
#include <Containers/String.h>

namespace Nova
{
    struct CommandLineOptionPossibleValue
    {
        String Name;
        String HelpText;
    };

    struct CommandLineOptionPossibleValueList
    {
        String Name;
        Array<CommandLineOptionPossibleValue> Values;
    };

    struct CommandLineOption
    {
        String::CharacterType ShortName;
        String LongName;
        bool Required;
        bool AllowMultiple;
        String HelpText;
        CommandLineOptionPossibleValueList PossibleValues;
    };
}