#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Backends::Win32
{
	extern bool initialized;

	void Initialize(HWND window);

	void NewFrame();

	void Shutdown();
}