#include "Logger.h"

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using std::string;
using std::cout;
using std::endl;

namespace Log
{
    FILE* consoleIn;
    FILE* consoleOut;
    FILE* consoleErr;

    void OpenConsole()
    {
        AllocConsole();

        freopen_s(&consoleIn, "CONIN$", "r", stdin);
        freopen_s(&consoleOut, "CONOUT$", "w", stdout);
        freopen_s(&consoleErr, "CONOUT$", "w", stderr);
    }

    void CloseConsole()
    {
        fclose(consoleIn);
        fclose(consoleOut);
        fclose(consoleErr);

        FreeConsole();
    }

    std::tm LocalTime_Xp(std::time_t time)
    {
        std::tm bt{};
        localtime_s(&bt, &time);
        return bt;
    }

    string TimeStamp(const std::string& fmt)
    {
        auto bt = LocalTime_Xp(std::time(0));
        char buf[64];
        return { buf, std::strftime(buf, sizeof(buf), fmt.c_str(), &bt) };
    }

    void Log::Log(std::string message, bool timestamp)
    {
        time_t now = time(nullptr);

        string modified;

        if (timestamp)
        {
            modified += "[" + TimeStamp("%T") + "] ";
        }

        modified += message;

        cout << modified << endl;

    }
}
    