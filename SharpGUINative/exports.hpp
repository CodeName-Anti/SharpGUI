#pragma once

namespace CSharpInterop
{
	typedef void (*CSharpCallback)();

	extern CSharpCallback initImGuiCallback;
	extern CSharpCallback renderCallback;
}

extern "C"
{
	__declspec(dllexport) bool __stdcall InitializeSharpGUI();
	__declspec(dllexport) bool __stdcall ShutdownSharpGUI();

	__declspec(dllexport) void __stdcall SetInitImGuiCallback(CSharpInterop::CSharpCallback initImGuiCallback);
	__declspec(dllexport) void __stdcall SetRenderCallback(CSharpInterop::CSharpCallback renderCallback);
}