#pragma once
#include "Runtime/DynamicLibrary.h"
#include <Windows.h>

namespace Nova
{
    DynamicLibrary::DynamicLibrary(const Path& Filepath) : m_Filepath(Filepath)
    {
    }

    bool DynamicLibrary::Initialize()
    {
        m_Handle = LoadLibrary(m_Filepath.c_str());
        if (!m_Handle)
            return false;
        return true;
    }

    void DynamicLibrary::Destroy() const
    {
        FreeLibrary((HMODULE)m_Handle);
    }

    void* DynamicLibrary::GetSymbol(const StringView& Name) const
    {
        return GetProcAddress((HMODULE)m_Handle, *Name);
    }
}