#include "win32_backend.hpp"

#define CIMGUI_USE_WIN32
#include "cimgui/cimgui_impl.h"

namespace Backends::Win32
{
	bool initialized = false;
	HWND window = NULL;
}

WNDPROC oWndProc;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

void Backends::Win32::Initialize(HWND window)
{
	Backends::Win32::window = window;
	oWndProc = (WNDPROC)SetWindowLongPtr(Backends::Win32::window, GWLP_WNDPROC, (LONG_PTR)WndProc);

	ImGui_ImplWin32_EnableDpiAwareness();
	ImGui_ImplWin32_Init(Backends::Win32::window);

	initialized = true;
}

void Backends::Win32::NewFrame()
{
	ImGui_ImplWin32_NewFrame();
}

void Backends::Win32::Shutdown()
{
	ImGui_ImplWin32_Shutdown();

	Backends::Win32::window = NULL;
	initialized = false;
}