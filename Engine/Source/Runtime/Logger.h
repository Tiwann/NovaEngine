#pragma once
#include "LogVerbosity.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Containers/StringFormat.h"

namespace Nova
{
    enum class ConsoleColor
    {
        Black = 30,	
        Red = 31,	
        Green = 32,	
        Yellow = 33,	
        Blue = 34,	
        Magenta = 35,	
        Cyan = 36,	
        White = 37,
        BrightBlack = 90,
        BrightRed = 91,
        BrightGreen = 92,
        BrightYellow = 93,
        BrightBlue = 94,
        BrightMagenta = 95,
        BrightCyan = 96,
        BrightWhite = 97,
        Reset
    };

    static StringView GetConsoleColorCode(ConsoleColor Color)
    {
        switch (Color)
        {
        case ConsoleColor::Black: return "\033[30m";
        case ConsoleColor::Red: return "\033[31m";
        case ConsoleColor::Green: return "\033[32m";
        case ConsoleColor::Yellow: return "\033[33m";
        case ConsoleColor::Blue: return "\033[34m";
        case ConsoleColor::Magenta: return "\033[35m";
        case ConsoleColor::Cyan: return "\033[36m";
        case ConsoleColor::White: return "\033[37m";
        case ConsoleColor::BrightBlack: return "\033[90m";
        case ConsoleColor::BrightRed: return "\033[91m";
        case ConsoleColor::BrightGreen: return "\033[92m";
        case ConsoleColor::BrightYellow: return "\033[93m";
        case ConsoleColor::BrightBlue: return "\033[94m";
        case ConsoleColor::BrightMagenta: return "\033[95m";
        case ConsoleColor::BrightCyan: return "\033[96m";
        case ConsoleColor::BrightWhite: return "\033[97m";
        case ConsoleColor::Reset: return "\033[0m";
        }
        return nullptr;
    }

    
    class Logger
    {
    public:
        explicit Logger(String&& name) : m_Name(std::move(name)){}

        template<typename... Args>
        void LogTrace(const String& format, const Args&... args)
        {
            const StringView code(GetConsoleColorCode(ConsoleColor::Reset));
            const String message = StringFormat(format, args...);
            if (m_CurrentCategory.IsValid())
                std::cout << StringFormat("{}[{}] [{}] {}\n", code, m_Name, *m_CurrentCategory, message);
            else
                std::cout << StringFormat("{}[{}] {}\n", code, m_Name, message);
        }

        template<typename... Args>
        void LogError(const String& format, const Args&... args)
        {
            const StringView code(GetConsoleColorCode(ConsoleColor::Red));
            const String message = StringFormat(format, args...);
            if (m_CurrentCategory.IsValid())
                std::cout << StringFormat("{}[{}] [{}] {}\n", code, m_Name, *m_CurrentCategory, message);
            else
                std::cout << StringFormat("{}[{}] {}\n", code, m_Name, message);
        }

        template<typename... Args>
        void LogWarning(const String& format, const Args&... args)
        {
            const StringView code(GetConsoleColorCode(ConsoleColor::Yellow));
            const String message = StringFormat(format, args...);
            if (m_CurrentCategory.IsValid())
                std::cout << StringFormat("{}[{}] [{}] {}\n", code, m_Name, *m_CurrentCategory, message);
            else
                std::cout << StringFormat("{}[{}] {}\n", code, m_Name, message);
        }

        template<typename... Args>
        void LogInfo(const String& format, const Args&... args)
        {
            const StringView code(GetConsoleColorCode(ConsoleColor::Green));
            const String message = StringFormat(format, args...);
            if (m_CurrentCategory.IsValid())
                std::cout << StringFormat("{}[{}] [{}] {}\n", code, m_Name, *m_CurrentCategory, message);
            else
                std::cout << StringFormat("{}[{}] {}\n", code, m_Name, message);
        }

        template<typename... Args>
        void Log(const Verbosity verbosity, const String& format, const Args&... args)
        {
            switch (verbosity)
            {
            case Verbosity::Trace:
                LogTrace(format, args...);
                break;
            case Verbosity::Info:
                LogInfo(format, args...);
                break;
            case Verbosity::Warning:
                LogWarning(format, args...);
                break;
            case Verbosity::Error:
                LogError(format, args...);
                break;
            }
        }

        void SetCurrentCategory(const String& category)
        {
            m_CurrentCategory = category;
        }
    private:
        String m_Name;
        StringView m_CurrentCategory = nullptr;
    };
}
