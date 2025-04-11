#pragma once
#include "Runtime/Filesystem.h"

namespace Nova
{
    class Project
    {
    public:
        explicit Project(const Path& Directory);

        const Path& GetDirectory() const;
        const Path& GetAssetsDirectory() const;
        const Path& GetShadersDirectory() const;
    private:
        Path m_Directory;
        Path m_AssetsDirectory;
        Path m_ShadersDirectory;
    };
}