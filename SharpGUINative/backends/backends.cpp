#include "backends.hpp"

#include "sharpgui.hpp"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"

#include "exports.hpp"

#include <string>

namespace Backends
{
	Backend* currentBackend;
}

Backends::BackendType Backends::Backend::GetType()
{
	return BackendType::BackendType_None;
}

void Backends::Backend::Initialize()
{
	if (this->initialized)
		return;

	this->InitializeBackend();

	initialized = true;
}

bool Backends::Backend::IsInitialized()
{
	return this->initialized;
}

void Backends::Backend::Shutdown()
{
	if (!this->initialized)
		return;

	this->ShutdownBackend();

	initialized = false;
}

bool Backends::Backend::GetHandleInput()
{
	return this->handleInput;
}

void Backends::InitImGui()
{
	igCreateContext(nullptr);
	ImGuiIO* io = igGetIO();
	io->ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard;
	io->IniFilename = nullptr;

	if (Interop::initImGuiCallback != nullptr)
		Interop::initImGuiCallback();
}

void Backends::ShutdownImGui()
{
	igDestroyContext(nullptr);
}

void Backends::RenderGUI()
{
	if (Interop::renderCallback != nullptr)
		Interop::renderCallback();

#if _DEBUG
	static bool showGui = true;

	// H is the greatest letter of all times
	if (igIsKeyPressed_Bool(ImGuiKey::ImGuiKey_H, false))
	{
		showGui = !showGui;
		SetHandleInput(showGui);
	}

	if (!showGui)
		return;

	igBegin("Debug h", nullptr, NULL);

	ImGuiIO* io = igGetIO();

	igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);

	igText("Shutdown will be fixed in a later version");

	if (igButton("Shutdown", ImVec2()))
	{
		ShutdownSharpGUI();
		return;
	}

	igEnd();
#endif
}