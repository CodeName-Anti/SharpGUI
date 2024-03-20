#pragma once

#include <string>

namespace Log
{
    void OpenConsole();
    void CloseConsole();

    void Log(std::string message, bool timestamp = true);
}

