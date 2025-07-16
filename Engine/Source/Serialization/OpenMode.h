#pragma once
#include "Runtime/Flags.h"

namespace Nova
{
    enum class OpenModeFlagBits
    {
        None = 0,
        Read = BIT(0),
        Write = BIT(1),
        Text = BIT(2),
        Binary = BIT(3),

        ReadText = Read | Text,
        WriteText = Write | Text,
        ReadBinary = Read | Binary,
        WriteBinary = Write | Binary,
        ReadWriteText = ReadText | ReadBinary,
        ReadWriteBinary = ReadText | WriteBinary,
    };

    typedef Flags<OpenModeFlagBits> OpenModeFlags;
}