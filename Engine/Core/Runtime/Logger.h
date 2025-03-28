#pragma once
#include "LogVerbosity.h"
#include "Containers/String.h"
#include "Containers/StringFormat.h"
#include <CoreExport.h>

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

    static const char* CORE_HIDDEN GetConsoleColorCode(ConsoleColor Color)
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
        Logger(String&& Name) : m_Name(std::move(Name)){}

        template<typename... Args>
        void LogTrace(const String& Fmt, const Args&... Arguments)
        {
            const StringView Code(GetConsoleColorCode(ConsoleColor::Reset));
            const String Message = Format(Fmt, Arguments...);
            std::cout << Format("{}[{}] [{}] {}\n", Code, m_Name, *m_CurrentCategory, Message);
        }

        template<typename... Args>
        void LogError(const String& Fmt, const Args&... Arguments)
        {
            const StringView Code(GetConsoleColorCode(ConsoleColor::Red));
            const String Message = Format(Fmt, Arguments...);
            std::cout << Format("{}[{}] [{}] {}\n", Code, m_Name, *m_CurrentCategory, Message);
        }

        template<typename... Args>
        void LogWarning(const String& Fmt, const Args&... Arguments)
        {
            const StringView Code(GetConsoleColorCode(ConsoleColor::Yellow));
            const String Message = Format(Fmt, Arguments...);
            std::cout << Format("{}[{}] [{}] {}\n", Code, m_Name, *m_CurrentCategory, Message);
        }

        template<typename... Args>
        void LogInfo(const String& Fmt, const Args&... Arguments)
        {
            const StringView Code(GetConsoleColorCode(ConsoleColor::Green));
            const String Message = Format(Fmt, Arguments...);
            std::cout << Format("{}[{}] [{}] {}\n", Code, m_Name, *m_CurrentCategory, Message);
        }

        template<typename... Args>
        void Log(Verbosity Verb, const String& Fmt, const Args&... Arguments)
        {
            switch (Verb)
            {
            case Verbosity::Trace:
                LogTrace(Fmt, Arguments...);
                break;
            case Verbosity::Info:
                LogInfo(Fmt, Arguments...);
                break;
            case Verbosity::Warning:
                LogWarning(Fmt, Arguments...);
                break;
            case Verbosity::Error:
                LogError(Fmt, Arguments...);
                break;
            }
        }

        void SetCurrentCategory(const String& Category)
        {
            m_CurrentCategory = Category;
        }
    private:
        String m_Name;
        StringView m_CurrentCategory = nullptr;
    };
}
