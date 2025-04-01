#pragma once
#include "Runtime/Flags.h"

enum class OpenModeFlagBits
{
    None = 0,
    NOVA_FLAG_BIT(Read, 0),
    NOVA_FLAG_BIT(Write, 1),
    NOVA_FLAG_BIT(Text, 2),
    NOVA_FLAG_BIT(Binary, 3),
    ReadText = Read | Text,
    ReadBinary = Read | Binary,
    WriteText = Write | Text,
    WriteBinary = Write | Binary,
    ReadWriteText = Read | Write | Text,
    ReadWriteBinary = Read | Write | Binary,
};

NOVA_DECLARE_FLAGS(OpenModeFlagBits, OpenModeFlags);