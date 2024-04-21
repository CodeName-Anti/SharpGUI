#include "backends.hpp"

#define CIMGUI_USE_WIN32
#include "cimgui/cimgui_impl.h"

#include "minhook/include/MinHook.h"

typedef bool (WINAPI* GetCursorPosDef)(LPPOINT lpPoint);
typedef bool (WINAPI* SetCursorPosDef)(int X, int Y);

typedef int (WINAPI* ShowCursorDef)(bool bShow);

typedef BOOL(WINAPI* ClipCursorDef)(const RECT* lpRect);
typedef BOOL(WINAPI* GetClipCursorDef)(LPRECT lpRect);

GetCursorPosDef oGetCursorPos;
SetCursorPosDef oSetCursorPos;

ShowCursorDef oShowCursor;

ClipCursorDef oClipCursor;
GetClipCursorDef oGetClipCursor;

namespace Backends::Win32
{
	bool initialized = false;
	HWND window = NULL;

	WNDPROC oWndProc = nullptr;

	bool cursorVisible = false;
	bool resetCursorState = false;
	POINT cursorPos{};
	RECT cursorClip{};
}

bool IsCursorVisible()
{
	CURSORINFO cursorInfo;
	cursorInfo.cbSize = sizeof(cursorInfo);
	GetCursorInfo(&cursorInfo);

	return cursorInfo.flags == CURSOR_SHOWING;
}

int HideMouseCursor()
{
	int count;
	while ((count = oShowCursor(false)) >= 0);
	return count;
}

int ShowMouseCursor()
{
	int count;
	while ((count = oShowCursor(true)) <= 0);
	return count;
}

bool WINAPI hkGetCursorPos(LPPOINT lpPoint)
{
	if (Backends::handleInput)
	{
		if (lpPoint != nullptr)
		{
			(*lpPoint) = Backends::Win32::cursorPos;
		}

		return true;
	}

	return oGetCursorPos(lpPoint);
}

bool WINAPI hkSetCursorPos(int X, int Y)
{
	if (Backends::handleInput)
	{
		Backends::Win32::cursorPos.x = X;
		Backends::Win32::cursorPos.y = Y;

		return true;
	}

	return oSetCursorPos(X, Y);
}

int WINAPI hkShowCursor(bool bShow)
{
	Backends::Win32::cursorVisible = bShow;

	if (Backends::handleInput)
	{
		return ShowMouseCursor();
	}

	int count = oShowCursor(bShow);

	return count;
}

BOOL WINAPI hkClipCursor(const RECT* lpRect)
{
	if (lpRect != nullptr)
	{
		Backends::Win32::cursorClip.left = lpRect->left;
		Backends::Win32::cursorClip.top = lpRect->top;
		Backends::Win32::cursorClip.right = lpRect->right;
		Backends::Win32::cursorClip.bottom = lpRect->bottom;
	}

	if (Backends::handleInput)
	{
		return true;
	}

	return oClipCursor(lpRect);
}

BOOL WINAPI hkGetClipCursor(LPRECT lpRect)
{
	if (Backends::handleInput)
	{
		*lpRect = Backends::Win32::cursorClip;
		return true;
	}

	return oGetClipCursor(lpRect);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (Backends::handleInput)
	{
		if (!IsCursorVisible())
		{
			ShowMouseCursor();
		}

		oClipCursor(nullptr);

		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		bool callOriginal = false;

		switch (uMsg)
		{
		case WM_SIZE:
		case WM_CLOSE:
		case WM_MOVE:
		case WM_WINDOWPOSCHANGING:
		case WM_WINDOWPOSCHANGED:
		case WM_ACTIVATEAPP:
		case WM_NCACTIVATE:
		case WM_ACTIVATE:
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		case WM_NCLBUTTONDOWN:
		case WM_NCLBUTTONUP:
		case WM_PAINT:
		case WM_SYSCOMMAND:
		case WM_NCLBUTTONDBLCLK:
		case WM_GETMINMAXINFO:
		case WM_NCCALCSIZE:
		case WM_NCPAINT:
		case WM_ERASEBKGND:
		case WM_MOVING:
		case WM_QUERYOPEN:
		case WM_GETICON:
		case WM_SIZING:
		case WM_SHOWWINDOW:
		case WM_DESTROY:
		case WM_NCDESTROY:
		case WM_NCHITTEST:
		case WM_NCMOUSEMOVE:
		case WM_SETCURSOR:
			callOriginal = true;
			break;

		default:
			break;
		}

		if (callOriginal)
			return CallWindowProc(Backends::Win32::oWndProc, hWnd, uMsg, wParam, lParam);
		else
			return true;
	}

	// Handle input keys when handleInput is false to still detect if a key is pressed
	switch (uMsg)
	{
	case WM_CHAR:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		break;

	default:
		break;
	}

	if (Backends::Win32::resetCursorState)
	{
		int count;

		if (Backends::Win32::cursorVisible != IsCursorVisible())
		{
			if (Backends::Win32::cursorVisible)
				count = ShowMouseCursor();
			else
				count = HideMouseCursor();
		}

		oClipCursor(&Backends::Win32::cursorClip);
		oSetCursorPos(Backends::Win32::cursorPos.x, Backends::Win32::cursorPos.y);

		Backends::Win32::resetCursorState = false;
	}

	return CallWindowProc(Backends::Win32::oWndProc, hWnd, uMsg, wParam, lParam);
}

void Backends::SetHandleInput(bool handleInput)
{
	if (Backends::handleInput == handleInput)
		return;

	Backends::handleInput = handleInput;

	if (handleInput)
	{
		Backends::Win32::cursorVisible = IsCursorVisible();
		oGetCursorPos(&Backends::Win32::cursorPos);
		oGetClipCursor(&Backends::Win32::cursorClip);
	}
	else
	{
		Backends::Win32::resetCursorState = true;
	}
}

void CreateAndEnableHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal)
{
	MH_CreateHook(pTarget, pDetour, ppOriginal);
	MH_EnableHook(pTarget);
}

void DisableAndRemoveHook(LPVOID pTarget)
{
	MH_DisableHook(pTarget);
	MH_RemoveHook(pTarget);
}

void Backends::Win32::Initialize(HWND window)
{
	Backends::Win32::window = window;
	oWndProc = (WNDPROC)SetWindowLongPtr(Backends::Win32::window, GWLP_WNDPROC, (LONG_PTR)WndProc);

	ImGui_ImplWin32_EnableDpiAwareness();
	ImGui_ImplWin32_Init(Backends::Win32::window);

	MH_Initialize();

	CreateAndEnableHook(&GetCursorPos, &hkGetCursorPos, (void**)&oGetCursorPos);
	CreateAndEnableHook(&SetCursorPos, &hkSetCursorPos, (void**)&oSetCursorPos);

	CreateAndEnableHook(&ShowCursor, &hkShowCursor, (void**)&oShowCursor);

	CreateAndEnableHook(&ClipCursor, &hkClipCursor, (void**)&oClipCursor);
	CreateAndEnableHook(&GetClipCursor, &hkGetClipCursor, (void**)&oGetClipCursor);

	initialized = true;
}

void Backends::Win32::NewFrame()
{
	ImGui_ImplWin32_NewFrame();
}

void Backends::Win32::Shutdown()
{
	(WNDPROC)SetWindowLongPtr(Backends::Win32::window, GWLP_WNDPROC, (LONG_PTR)oWndProc);

	ImGui_ImplWin32_Shutdown();

	DisableAndRemoveHook(&GetCursorPos);
	DisableAndRemoveHook(&SetCursorPos);

	DisableAndRemoveHook(&ShowCursor);

	DisableAndRemoveHook(&ClipCursor);
	DisableAndRemoveHook(&GetClipCursor);

	Backends::Win32::window = NULL;
	initialized = false;
}
