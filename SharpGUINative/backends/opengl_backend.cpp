#include "sharpconfig.hpp"

#if SHARPGUI_INCLUDE_OPENGL

#include "backends.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "minhook/include/MinHook.h"

#define CIMGUI_USE_OPENGL2
#define CIMGUI_USE_OPENGL3
#include "cimgui/cimgui_impl.h"

#define IMGL3W_IMPL
#include "cimgui/imgui_impl_opengl3_loader.h"

#include "Logger/Logger.h"

#define GL_SHADING_LANGUAGE_VERSION 0x8B8C

typedef bool(__stdcall* WglSwapBuffersFunc)(HDC hDc);

namespace Backends::OpenGL
{
	bool initialized = false;
	HWND window = NULL;

	bool initHook = false;
	bool newOpenGL = false;
	
	WglSwapBuffersFunc* swapBuffersAddress = NULL;
}

WglSwapBuffersFunc oWglSwapBuffers;
HGLRC wglContext;

bool __stdcall hkWglSwapBuffers(HDC hDc)
{
	if (!Backends::OpenGL::initHook)
	{
		if (WindowFromDC(hDc) == Backends::OpenGL::window)
			return oWglSwapBuffers(hDc);

		Backends::OpenGL::window = WindowFromDC(hDc);

		Backends::InitImGui();
		Backends::Win32::Initialize(Backends::OpenGL::window);

		int res = imgl3wInit();

		if (res != 0)
		{
			Log::Log("loader cant init code: " + res);
		}

		GLint iMajor;
		GLint iMinor;

		glGetIntegerv(GL_MAJOR_VERSION, &iMajor);
		glGetIntegerv(GL_MINOR_VERSION, &iMinor);

		// Check if OpenGL version is above 3.0
		if ((iMajor * 10 + iMinor) >= 30)
			Backends::OpenGL::newOpenGL = true;

		if (Backends::OpenGL::newOpenGL)
		{
			ImGui_ImplOpenGL3_Init(nullptr);
		}
		else
		{
			ImGui_ImplOpenGL2_Init();
		}

		wglContext = wglCreateContext(hDc);

		Backends::OpenGL::initHook = true;
	}

	HGLRC originalWglContext = wglGetCurrentContext();

	wglMakeCurrent(hDc, wglContext);

	if (Backends::OpenGL::newOpenGL)
		ImGui_ImplOpenGL3_NewFrame();
	else
		ImGui_ImplOpenGL2_NewFrame();
	
	Backends::Win32::NewFrame();
	igNewFrame();

	Backends::RenderGUI();

	igEndFrame();
	igRender();
	
	if (Backends::OpenGL::newOpenGL)
		ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
	else
		ImGui_ImplOpenGL2_RenderDrawData(igGetDrawData());

	wglMakeCurrent(hDc, originalWglContext);

	return oWglSwapBuffers(hDc);
}

WglSwapBuffersFunc* GetWglSwapBuffers()
{
	auto hMod = GetModuleHandleA("OPENGL32.dll");
	if (!hMod) return nullptr;

	return (WglSwapBuffersFunc*)GetProcAddress(hMod, "wglSwapBuffers");
}

void Backends::OpenGL::Initialize()
{
	if (initialized)
		return;

	Backends::OpenGL::swapBuffersAddress = GetWglSwapBuffers();

	MH_CreateHook(Backends::OpenGL::swapBuffersAddress, hkWglSwapBuffers, (void**)&oWglSwapBuffers);
	MH_EnableHook(Backends::OpenGL::swapBuffersAddress);

	initialized = true;
}

void Backends::OpenGL::Shutdown()
{
	if (!initialized)
		return;

	MH_DisableHook(Backends::OpenGL::swapBuffersAddress);
	MH_RemoveHook(Backends::OpenGL::swapBuffersAddress);

	if (Backends::OpenGL::newOpenGL)
	{
		ImGui_ImplOpenGL3_Shutdown();
	}
	else
	{
		ImGui_ImplOpenGL2_Shutdown();
	}

	wglDeleteContext(wglContext);

	Backends::Win32::Shutdown();

	Backends::ShutdownImGui();

	Backends::OpenGL::initHook = false;
	initialized = false;
}
#endif