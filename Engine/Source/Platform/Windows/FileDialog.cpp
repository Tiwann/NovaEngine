#include "Containers/StringConversion.h"
#include "Runtime/Application.h"
#include "Runtime/DesktopWindow.h"
#include "Runtime/Path.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <Windows.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <filesystem>

namespace Nova
{
    String Path::OpenFileDialog(const StringView title, const StringView defaultPath, const DialogFilters& filters, Window& owningWindow)
    {
        OPENFILENAMEW openFilename = { };
        Memory::Memzero(openFilename);
        WCHAR szFile[MAX_PATH] = { 0 };
        openFilename.hwndOwner = glfwGetWin32Window(((DesktopWindow&)owningWindow).GetHandle());
        openFilename.lStructSize = sizeof(OPENFILENAMEW);
        openFilename.lpstrFile = szFile;
        openFilename.nMaxFile = MAX_PATH;
        openFilename.nFilterIndex = 1;
        openFilename.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;

        const String filterString = filters.GetFilterString();
        const WideString filterWideString = StringConvertToWide(filterString);
        openFilename.lpstrFilter = *filterWideString;

        const WideString wideDefaultPath = StringConvertToWide(defaultPath);
        openFilename.lpstrInitialDir = *wideDefaultPath;

        const WideString wideTitle = StringConvertToWide(title);
        openFilename.lpstrTitle = *wideTitle;

        const bool result = GetOpenFileNameW(&openFilename);

        String resultPath = StringConvertToMultibyte(WideStringView(openFilename.lpstrFile));
        return result ? *resultPath : "";
    }

    String Path::SaveFileDialog(StringView title, StringView defaultPath, const DialogFilters& filters, Window& owningWindow)
    {
        OPENFILENAME openFilename = { };
        Memory::Memzero(openFilename);
        WCHAR szFile[MAX_PATH] = { 0 };
        openFilename.hwndOwner = glfwGetWin32Window(((DesktopWindow&)owningWindow).GetHandle());
        openFilename.lStructSize = sizeof(OPENFILENAME);
        openFilename.lpstrFile = szFile;
        openFilename.nMaxFile = MAX_PATH;
        openFilename.nFilterIndex = 1;
        openFilename.Flags = OFN_CREATEPROMPT | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT | OFN_NOLONGNAMES | OFN_EXTENSIONDIFFERENT | OFN_EXPLORER;

        const String filterString = filters.GetFilterString();
        const WideString filterWideString = StringConvertToWide(filterString);
        openFilename.lpstrFilter = *filterWideString;

        const WideString wideDefaultPath = StringConvertToWide(defaultPath);
        openFilename.lpstrInitialDir = *wideDefaultPath;

        const WideString wideTitle = StringConvertToWide(title);
        openFilename.lpstrTitle = *wideTitle;

        const bool result = GetOpenFileNameW(&openFilename);

        String resultPath = StringConvertToMultibyte(WideStringView(openFilename.lpstrFile));
        return result ? *resultPath : "";
    }

    bool Path::Exists(StringView path)
    {
        return std::filesystem::exists({*path});
    }

    bool Path::IsFile(StringView path)
    {
        return !IsDirectory(path);
    }

    bool Path::IsDirectory(StringView path)
    {
        return std::filesystem::is_directory({*path});
    }

    Array<String> Path::GetFiles(StringView path)
    {
        if (!IsDirectory(path)) return {};
        std::filesystem::path directory(*path);
        Array<String> files;
        for (auto it : std::filesystem::directory_iterator(directory))
            files.Add(it.path().string().c_str());
        return files;
    }
}
