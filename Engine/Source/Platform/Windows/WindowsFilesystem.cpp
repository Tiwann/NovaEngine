#include "Runtime/Filesystem.h"
#include "Runtime/Window.h"
#include "Runtime/Application.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <commdlg.h>
#include <processenv.h>

namespace Nova
{
    Path Directory::GetUserDirectory()
    {
        CHAR DirectoryPath[MAX_PATH] = { 0 };
        const DWORD DirectoryPathLength = GetEnvironmentVariableA("USERPROFILE", DirectoryPath, MAX_PATH);
        const std::string_view DirectoryStr(DirectoryPath, DirectoryPathLength);
        return DirectoryStr;
    }
    
    Path File::OpenFileDialog(const String& Title, const Path& DefaultPath, const DialogFilters& Filters)
    {
        Window* Window = g_Application->GetWindow();
        OPENFILENAMEA OpenFileName = { };
        Memory::Memzero(OpenFileName);
        CHAR szFile[MAX_PATH] = { 0 };
        OpenFileName.hwndOwner = glfwGetWin32Window(Window->GetNativeWindow());
        OpenFileName.lStructSize = sizeof(OPENFILENAME);
        OpenFileName.lpstrFile = szFile;
        OpenFileName.nMaxFile = MAX_PATH;
        ScopedBuffer<char> FilterString = Filters.GetFilterString();
        OpenFileName.lpstrFilter = FilterString.GetData();
        OpenFileName.nFilterIndex = 1;
        OpenFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;
        OpenFileName.lpstrInitialDir = DefaultPath.string().c_str();
        OpenFileName.lpstrTitle = *Title;
        return GetOpenFileNameA(&OpenFileName) ? OpenFileName.lpstrFile : "";
    }

    Path File::SaveFileDialog(const String& Title, const Path& DefaultPath, const DialogFilters& Filters)
    {
        Window* Window = g_Application->GetWindow();
        OPENFILENAMEA OpenFileName = { };
        ZeroMemory(&OpenFileName, sizeof(OPENFILENAME));
        CHAR szFile[MAX_PATH] = { 0 };
        OpenFileName.hwndOwner = glfwGetWin32Window(Window->GetNativeWindow());
        OpenFileName.lStructSize = sizeof(OPENFILENAME);
        OpenFileName.lpstrFile = szFile;
        OpenFileName.nMaxFile = MAX_PATH;
        ScopedBuffer<char> FilterString = Filters.GetFilterString();
        OpenFileName.lpstrFilter = FilterString.GetData();
        OpenFileName.nFilterIndex = 1;
        OpenFileName.Flags = OFN_CREATEPROMPT | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT | OFN_NOLONGNAMES | OFN_EXTENSIONDIFFERENT |OFN_EXPLORER;
        OpenFileName.lpstrInitialDir = DefaultPath.string().c_str();
        OpenFileName.lpstrTitle = *Title;
        return GetSaveFileNameA(&OpenFileName) ? OpenFileName.lpstrFile : "";
    }
        Path Directory::OpenFolderDialog(const String& Title, const Path& DefaultPath)
    {
            Window* Window = g_Application->GetWindow();
            OPENFILENAMEA OpenFileName = { };
            ZeroMemory(&OpenFileName, sizeof(OPENFILENAME));
            CHAR szFile[MAX_PATH] = { 0 };
            OpenFileName.hwndOwner = glfwGetWin32Window(Window->GetNativeWindow());
            OpenFileName.lStructSize = sizeof(OPENFILENAME);
            OpenFileName.lpstrFile = szFile;
            OpenFileName.nMaxFile = MAX_PATH;
            OpenFileName.nFilterIndex = 1;
            OpenFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;
            OpenFileName.lpstrInitialDir = DefaultPath.string().c_str();
            OpenFileName.lpstrTitle = *Title;
            return GetOpenFileNameA(&OpenFileName) ? OpenFileName.lpstrFile : "";
    }

        Path Directory::GetCurrentWorkingDirectory()
        {
            WCHAR CurrentDir[MAX_PATH] = { 0 };
            GetCurrentDirectoryW(MAX_PATH, CurrentDir);
            return CurrentDir;
        }
}
