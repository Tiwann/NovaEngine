#include "Runtime/Filesystem.h"
#include "Runtime/Window.h"
#include "Runtime/Application.h"
#include "Containers/StringConversion.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <commdlg.h>
#include <processenv.h>

#include "Runtime/DesktopWindow.h"

namespace Nova
{
    Path Directory::GetUserDirectory()
    {
        WCHAR DirectoryPath[MAX_PATH] = { 0 };
        const DWORD DirectoryPathLength = GetEnvironmentVariableW(L"USERPROFILE", DirectoryPath, MAX_PATH);
        return Path(DirectoryPath, DirectoryPath + DirectoryPathLength);
    }
    
    Path File::OpenFileDialog(const String& Title, const Path& DefaultPath, const DialogFilters& Filters)
    {
        Window* Window = g_Application->GetWindow();
        OPENFILENAME OpenFileName = { };
        Memory::Memzero(OpenFileName);
        WCHAR szFile[MAX_PATH] = { 0 };
        OpenFileName.hwndOwner = glfwGetWin32Window(Window->As<DesktopWindow>()->GetHandle());
        OpenFileName.lStructSize = sizeof(OPENFILENAME);
        OpenFileName.lpstrFile = szFile;
        OpenFileName.nMaxFile = MAX_PATH;
        ScopedBuffer<char> FilterString = Filters.GetFilterString();
        const i32 Length = MultiByteToWideChar(CP_UTF8, 0, FilterString.GetData(), FilterString.Count(), nullptr, 0);
        WideString::CharacterType* FilterWideString = new WideString::CharacterType[Length]{0};
        MultiByteToWideChar(CP_UTF8, 0, FilterString.GetData(), FilterString.Count(), FilterWideString, Length);

        OpenFileName.lpstrFilter = FilterWideString;
        OpenFileName.nFilterIndex = 1;
        OpenFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;
        OpenFileName.lpstrInitialDir = DefaultPath.wstring().c_str();

        WideString WideTitle = StringConvertToWide(Title);
        OpenFileName.lpstrTitle = *WideTitle;

        const bool Result = GetOpenFileNameW(&OpenFileName);
        delete[] FilterWideString;
        return Result ? OpenFileName.lpstrFile : L"";
    }

    Path File::SaveFileDialog(const String& Title, const Path& DefaultPath, const DialogFilters& Filters)
    {
        Window* Window = g_Application->GetWindow();
        OPENFILENAME OpenFileName = { };
        Memory::Memzero(OpenFileName);
        WCHAR szFile[MAX_PATH] = { 0 };
        OpenFileName.hwndOwner = glfwGetWin32Window(Window->As<DesktopWindow>()->GetHandle());
        OpenFileName.lStructSize = sizeof(OPENFILENAME);
        OpenFileName.lpstrFile = szFile;
        OpenFileName.nMaxFile = MAX_PATH;
        ScopedBuffer<char> FilterString = Filters.GetFilterString();
        const i32 Length = MultiByteToWideChar(CP_UTF8, 0, FilterString.GetData(), FilterString.Count(), nullptr, 0);
        WideString::CharacterType* FilterWideString = new WideString::CharacterType[Length] { 0 };
        MultiByteToWideChar(CP_UTF8, 0, FilterString.GetData(), FilterString.Count(), FilterWideString, Length);

        OpenFileName.lpstrFilter = FilterWideString;
        OpenFileName.nFilterIndex = 1;
        OpenFileName.Flags = OFN_CREATEPROMPT | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT | OFN_NOLONGNAMES | OFN_EXTENSIONDIFFERENT |OFN_EXPLORER;

        WideString WideTitle = StringConvertToWide(Title);
        OpenFileName.lpstrTitle = *WideTitle;

        OpenFileName.lpstrInitialDir = DefaultPath.wstring().c_str();
        OpenFileName.lpstrTitle = *WideTitle;
        const bool Result = GetSaveFileNameW(&OpenFileName);
        delete[] FilterWideString;
        return Result ? OpenFileName.lpstrFile : L"";
    }

    Path Directory::OpenFolderDialog(const String& Title, const Path& DefaultPath)
    {
        Window* Window = g_Application->GetWindow();
        OPENFILENAME OpenFileName = { };
        Memory::Memzero(OpenFileName);
        WCHAR szFile[MAX_PATH] = { 0 };
        OpenFileName.hwndOwner = glfwGetWin32Window(Window->As<DesktopWindow>()->GetHandle());
        OpenFileName.lStructSize = sizeof(OPENFILENAME);
        OpenFileName.lpstrFile = szFile;
        OpenFileName.nMaxFile = MAX_PATH;
        OpenFileName.nFilterIndex = 1;
        OpenFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;
        OpenFileName.lpstrInitialDir = DefaultPath.wstring().c_str();

        WideString WideTitle = StringConvertToWide(Title);
        OpenFileName.lpstrTitle = *WideTitle;
        const bool Result = GetOpenFileNameW(&OpenFileName);
        return Result ? OpenFileName.lpstrFile : L"";
    }

    Path Directory::GetCurrentWorkingDirectory()
    {
        WCHAR CurrentDir[MAX_PATH] = { 0 };
        GetCurrentDirectoryW(MAX_PATH, CurrentDir);
        return CurrentDir;
    }
}
