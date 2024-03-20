#include "backends.hpp"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"

#include "exports.hpp"

void Backends::InitImGui()
{
	igCreateContext(nullptr);
	ImGuiIO* io = igGetIO();
	io->ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io->IniFilename = nullptr;

	if (CSharpInterop::initImGuiCallback != nullptr)
		CSharpInterop::initImGuiCallback();
}

void Backends::ShutdownImGui()
{
	igDestroyContext(nullptr);
}

void Backends::RenderGUI()
{
	if (CSharpInterop::renderCallback != nullptr)
		CSharpInterop::renderCallback();
}