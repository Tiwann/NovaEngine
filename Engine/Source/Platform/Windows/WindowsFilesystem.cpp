#include "Runtime/Filesystem.h"
#include "Runtime/Window.h"
#include "Runtime/Application.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <commdlg.h>
#include <processenv.h>

namespace Nova
{
    Path Directory::GetUserDirectory()
    {
        CHAR DirectoryPath[NOVA_FILENAME_MAX_LENGTH] = { 0 };
        const DWORD DirectoryPathLength = GetEnvironmentVariableA("USERPROFILE", DirectoryPath, NOVA_FILENAME_MAX_LENGTH);
        const std::string_view DirectoryStr(DirectoryPath, DirectoryPathLength);
        return DirectoryStr;
    }
    
    Path File::OpenFileDialog(const String& Title, const Path& DefaultPath, const DialogFilters& Filters)
    {
        Window* Window = g_Application->GetWindow();
        OPENFILENAMEA OpenFileName = { };
        Memory::Memzero(OpenFileName);
        CHAR szFile[NOVA_FILENAME_MAX_LENGTH] = { 0 };
        OpenFileName.hwndOwner = glfwGetWin32Window(Window->GetNativeWindow());
        OpenFileName.lStructSize = sizeof(OPENFILENAME);
        OpenFileName.lpstrFile = szFile;
        OpenFileName.nMaxFile = NOVA_FILENAME_MAX_LENGTH;
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
        CHAR szFile[NOVA_FILENAME_MAX_LENGTH] = { 0 };
        OpenFileName.hwndOwner = glfwGetWin32Window(Window->GetNativeWindow());
        OpenFileName.lStructSize = sizeof(OPENFILENAME);
        OpenFileName.lpstrFile = szFile;
        OpenFileName.nMaxFile = NOVA_FILENAME_MAX_LENGTH;
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
            CHAR szFile[NOVA_FILENAME_MAX_LENGTH] = { 0 };
            OpenFileName.hwndOwner = glfwGetWin32Window(Window->GetNativeWindow());
            OpenFileName.lStructSize = sizeof(OPENFILENAME);
            OpenFileName.lpstrFile = szFile;
            OpenFileName.nMaxFile = NOVA_FILENAME_MAX_LENGTH;
            OpenFileName.nFilterIndex = 1;
            OpenFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;
            OpenFileName.lpstrInitialDir = DefaultPath.string().c_str();
            OpenFileName.lpstrTitle = *Title;
            return GetOpenFileNameA(&OpenFileName) ? OpenFileName.lpstrFile : "";
    }

        Path Directory::GetCurrentWorkingDirectory()
        {
            WCHAR CurrentDir[NOVA_FILENAME_MAX_LENGTH] = { 0 };
            GetCurrentDirectory(NOVA_FILENAME_MAX_LENGTH, CurrentDir);
            return CurrentDir;
        }
}
