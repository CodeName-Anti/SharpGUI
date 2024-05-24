#include "win32_backend.hpp"
#include "backends.hpp"

#define CIMGUI_USE_WIN32
#include "cimgui/cimgui_impl.h"

#include "minhook/include/MinHook.h"

#include <map>

typedef bool (WINAPI* GetCursorPosDef)(LPPOINT lpPoint);
typedef bool (WINAPI* SetCursorPosDef)(int X, int Y);

typedef int (WINAPI* ShowCursorDef)(bool bShow);

typedef BOOL(WINAPI* ClipCursorDef)(const RECT* lpRect);
typedef BOOL(WINAPI* GetClipCursorDef)(LPRECT lpRect);

typedef HCURSOR(WINAPI* GetCursorDef)();
typedef HCURSOR(WINAPI* SetCursorDef)(HCURSOR hCursor);

namespace Backends::Win32
{
	bool initialized = false;
	bool handleInput = false;
	HWND window = NULL;

	WNDPROC oWndProc = nullptr;

	bool initCursor = false;
	bool resetCursorState = false;

	bool cursorVisible = false;
	POINT cursorPos{};
	RECT cursorClip{};
	HCURSOR cursor;

	// Utilities
	bool IsCursorVisible();
	int HideMouseCursor();
	int ShowMouseCursor();
	bool IsImguiCursor(HCURSOR hCursor);

	// Win API Hooks originals
	GetCursorPosDef oGetCursorPos;
	SetCursorPosDef oSetCursorPos;

	ShowCursorDef oShowCursor;

	ClipCursorDef oClipCursor;
	GetClipCursorDef oGetClipCursor;

	GetCursorDef oGetCursor;
	SetCursorDef oSetCursor;

	// Win API Hooks
	bool WINAPI hkGetCursorPos(LPPOINT lpPoint);
	bool WINAPI hkSetCursorPos(int X, int Y);

	int WINAPI hkShowCursor(bool bShow);

	BOOL WINAPI hkClipCursor(const RECT* lpRect);
	BOOL WINAPI hkGetClipCursor(LPRECT lpRect);

	HCURSOR WINAPI hkGetCursor();
	HCURSOR WINAPI hkSetCursor(HCURSOR hCursor);

	LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Hooking helpers
	void CreateAndEnableHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal);
	void DisableAndRemoveHook(LPVOID pTarget);
}

bool Backends::Win32::IsCursorVisible()
{
	CURSORINFO cursorInfo;
	cursorInfo.cbSize = sizeof(cursorInfo);
	GetCursorInfo(&cursorInfo);

	return cursorInfo.flags == CURSOR_SHOWING;
}

int Backends::Win32::HideMouseCursor()
{
	int count;
	while ((count = oShowCursor(false)) >= 0);
	return count;
}

int Backends::Win32::ShowMouseCursor()
{
	int count;
	while ((count = oShowCursor(true)) <= 0);
	return count;
}

bool Backends::Win32::IsImguiCursor(HCURSOR hCursor)
{
	static std::map<ImGuiMouseCursor, HCURSOR> cursorMap;

	if (cursorMap.size() == 0)
	{
#define ADD_CURSOR(imguiCursor, cursor) cursorMap[imguiCursor] = LoadCursor(nullptr, cursor);

		cursorMap[ImGuiMouseCursor_None] = nullptr;
		ADD_CURSOR(ImGuiMouseCursor_Arrow, IDC_ARROW);
		ADD_CURSOR(ImGuiMouseCursor_TextInput, IDC_IBEAM);
		ADD_CURSOR(ImGuiMouseCursor_ResizeAll, IDC_SIZEALL);
		ADD_CURSOR(ImGuiMouseCursor_ResizeEW, IDC_SIZEWE);
		ADD_CURSOR(ImGuiMouseCursor_ResizeNS, IDC_SIZENS);
		ADD_CURSOR(ImGuiMouseCursor_ResizeNESW, IDC_SIZENESW);
		ADD_CURSOR(ImGuiMouseCursor_ResizeNWSE, IDC_SIZENWSE);
		ADD_CURSOR(ImGuiMouseCursor_Hand, IDC_HAND);
		ADD_CURSOR(ImGuiMouseCursor_NotAllowed, IDC_NO);

#undef ADD_CURSOR
	}

	ImGuiMouseCursor imguiCursor = igGetMouseCursor();

	if (!cursorMap.contains(imguiCursor))
		return false;

	return cursorMap[imguiCursor] == hCursor;
}

bool WINAPI Backends::Win32::hkGetCursorPos(LPPOINT lpPoint)
{
	if (Backends::Win32::handleInput)
	{
		if (lpPoint != nullptr)
		{
			(*lpPoint) = Backends::Win32::cursorPos;
		}

		return true;
	}

	return oGetCursorPos(lpPoint);
}

bool WINAPI Backends::Win32::hkSetCursorPos(int X, int Y)
{
	if (Backends::Win32::handleInput)
	{
		Backends::Win32::cursorPos.x = X;
		Backends::Win32::cursorPos.y = Y;

		return true;
	}

	return oSetCursorPos(X, Y);
}

int WINAPI Backends::Win32::hkShowCursor(bool bShow)
{
	Backends::Win32::cursorVisible = bShow;

	if (Backends::Win32::handleInput)
	{
		return ShowMouseCursor();
	}

	int count = oShowCursor(bShow);

	return count;
}

BOOL WINAPI Backends::Win32::hkClipCursor(const RECT* lpRect)
{
	if (lpRect != nullptr)
	{
		Backends::Win32::cursorClip.left = lpRect->left;
		Backends::Win32::cursorClip.top = lpRect->top;
		Backends::Win32::cursorClip.right = lpRect->right;
		Backends::Win32::cursorClip.bottom = lpRect->bottom;
	}

	if (Backends::Win32::handleInput)
	{
		return true;
	}

	return oClipCursor(lpRect);
}

BOOL WINAPI Backends::Win32::hkGetClipCursor(LPRECT lpRect)
{
	if (Backends::Win32::handleInput)
	{
		*lpRect = Backends::Win32::cursorClip;
		return true;
	}

	return oGetClipCursor(lpRect);
}

HCURSOR WINAPI Backends::Win32::hkGetCursor()
{
	if (Backends::Win32::handleInput)
		return Backends::Win32::cursor;

	return oGetCursor();
}

HCURSOR WINAPI Backends::Win32::hkSetCursor(HCURSOR hCursor)
{
	HCURSOR oldCursor = Backends::Win32::cursor;
	Backends::Win32::cursor = hCursor;

	if (Backends::Win32::handleInput && !IsImguiCursor(hCursor))
	{
		return oldCursor;
	}

	return oSetCursor(hCursor);
}

LRESULT __stdcall Backends::Win32::WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (Backends::Win32::handleInput)
	{
		// Init cursor
		if (Backends::Win32::initCursor)
		{
			oSetCursor(LoadCursor(nullptr, IDC_ARROW));

			Backends::Win32::initCursor = false;
		}

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
		case WM_QUIT:
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

void Backends::Win32Backend::SetHandleInput(bool handleInput)
{
	if (Backends::Win32::handleInput == handleInput)
		return;

	this->handleInput = handleInput;
	Backends::Win32::handleInput = handleInput;

	if (handleInput)
	{
		Backends::Win32::cursorVisible = Backends::Win32::IsCursorVisible();
		Backends::Win32::oGetCursorPos(&Backends::Win32::cursorPos);
		Backends::Win32::oGetClipCursor(&Backends::Win32::cursorClip);

		Backends::Win32::cursor = Backends::Win32::oGetCursor();

		Backends::Win32::initCursor = true;
	}
	else
	{
		Backends::Win32::resetCursorState = true;
	}
}

void Backends::Win32::CreateAndEnableHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal)
{
	MH_CreateHook(pTarget, pDetour, ppOriginal);
	MH_EnableHook(pTarget);
}

void Backends::Win32::DisableAndRemoveHook(LPVOID pTarget)
{
	MH_DisableHook(pTarget);
	MH_RemoveHook(pTarget);
}

void Backends::Win32::Initialize(HWND window)
{
	if (initialized)
		return;

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

	CreateAndEnableHook(&GetCursor, &hkGetCursor, (void**)&oGetCursor);
	CreateAndEnableHook(&SetCursor, &hkSetCursor, (void**)&oSetCursor);

	initialized = true;
}

void Backends::Win32::NewFrame()
{
	if (!initialized)
		return;

	ImGui_ImplWin32_NewFrame();
}

void Backends::Win32::Shutdown()
{
	if (!initialized)
		return;

	(WNDPROC)SetWindowLongPtr(Backends::Win32::window, GWLP_WNDPROC, (LONG_PTR)oWndProc);

	ImGui_ImplWin32_Shutdown();

	DisableAndRemoveHook(&GetCursorPos);
	DisableAndRemoveHook(&SetCursorPos);

	DisableAndRemoveHook(&ShowCursor);

	DisableAndRemoveHook(&ClipCursor);
	DisableAndRemoveHook(&GetClipCursor);

	DisableAndRemoveHook(&GetCursor);
	DisableAndRemoveHook(&SetCursor);

	Backends::Win32::window = NULL;
	initialized = false;
}
