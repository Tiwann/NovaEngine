#include "Project.h"

namespace Nova
{
    Project::Project(const Path& Directory)
    : m_Directory(Directory),
    m_AssetsDirectory(m_AssetsDirectory / "Assets"),
    m_ShadersDirectory(m_AssetsDirectory / "Shaders")
    {
    }

    const Path& Project::GetDirectory() const
    {
        return m_Directory;
    }

    const Path& Project::GetAssetsDirectory() const
    {
        return m_AssetsDirectory;
    }

    const Path& Project::GetShadersDirectory() const
    {
        return m_ShadersDirectory;
    }
}
