#include "exports.hpp"
#include "sharpgui.hpp"

namespace CSharpInterop
{
	CSharpCallback initImGuiCallback;
	CSharpCallback renderCallback;
}

bool __stdcall InitializeSharpGUI()
{
	return SharpGUI::Initialize();
}

bool __stdcall InitializeSharpGUIBackend(int backendType)
{
	return SharpGUI::Initialize((Backends::BackendType::Enum)backendType);
}

bool __stdcall ShutdownSharpGUI()
{
	return SharpGUI::Shutdown();
}

void __stdcall SetInitImGuiCallback(CSharpInterop::CSharpCallback initImGuiCallback)
{
	CSharpInterop::initImGuiCallback = initImGuiCallback;
}

void __stdcall SetRenderCallback(CSharpInterop::CSharpCallback renderCallback)
{
	CSharpInterop::renderCallback = renderCallback;
}

void __stdcall SetHandleInput(bool handleInput)
{
	Backends::SetHandleInput(handleInput);
}
