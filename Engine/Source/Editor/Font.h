#pragma once
#include <imgui.h>

#include "Runtime/Application.h"

namespace Nova
{
    enum FontNames
    {
        // Open Sans
        OpenSans_Regular,
        OpenSans_Italic,
        OpenSans_Bold,
        OpenSans_SemiBold,
        OpenSans_ExtraBold,

        // Jetbrains Mono
        JetBrainsMono_Regular,
        JetBrainsMono_Italic,
        JetBrainsMono_Bold,
        JetBrainsMono_SemiBold,
        JetBrainsMono_ExtraBold,

        NumFonts
    };


    inline const char* FontsPaths[NumFonts]
    {
        // Open Sans
        "Fonts/OpenSans/OpenSans-Regular.ttf",
        "Fonts/OpenSans/OpenSans-Italic.ttf",
        "Fonts/OpenSans/OpenSans-Bold.ttf",
        "Fonts/OpenSans/OpenSans-SemiBold.ttf",
        "Fonts/OpenSans/OpenSans-ExtraBold.ttf",

        // JetBrainsMono
        "Fonts/JetBrainsMono/JetBrainsMono-Regular.ttf",
        "Fonts/JetBrainsMono/JetBrainsMono-Italic.ttf",
        "Fonts/JetBrainsMono/JetBrainsMono-Bold.ttf",
        "Fonts/JetBrainsMono/JetBrainsMono-SemiBold.ttf",
        "Fonts/JetBrainsMono/JetBrainsMono-ExtraBold.ttf",
    };

    inline Path GetFontPath(const FontNames Name)
    {
        return PathCombine(g_Application->GetEngineAssetsDirectory(), FontsPaths[Name]);
    }

    inline ImFont* Fonts[NumFonts];
}
