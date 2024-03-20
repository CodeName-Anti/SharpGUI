#include "sharpconfig.hpp"

#if SHARPGUI_INCLUDE_DX9

#include "backends.hpp"
#include "d3d9.h"
#include "kiero.h"

#define CIMGUI_USE_DX9
#include "cimgui/cimgui_impl.h"

namespace Backends::DX9
{
	bool initialized = false;
	bool hookInit = false;
}

typedef HRESULT(__stdcall* EndSceneFunc)(IDirect3DDevice9* pDevice);
typedef HRESULT(__stdcall* ResetFunc)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);

EndSceneFunc oEndScene;
ResetFunc oReset;

HRESULT __stdcall hkEndScene(IDirect3DDevice9* pDevice)
{
	if (!Backends::DX9::hookInit)
	{
		D3DDEVICE_CREATION_PARAMETERS creationParameters;
		pDevice->GetCreationParameters(&creationParameters);

		Backends::InitImGui();
		Backends::Win32::Initialize(creationParameters.hFocusWindow);
		ImGui_ImplDX9_Init(pDevice);

		Backends::DX9::hookInit = true;
	}

	Backends::Win32::NewFrame();
	ImGui_ImplDX9_NewFrame();
	igNewFrame();

	Backends::RenderGUI();

	igEndFrame();
	igRender();

	ImGui_ImplDX9_RenderDrawData(igGetDrawData());

	return oEndScene(pDevice);
}

HRESULT __stdcall hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	HRESULT hr = oReset(pDevice, pPresentationParameters);

	ImGui_ImplDX9_CreateDeviceObjects();

	return hr;
}

void Backends::DX9::Initialize()
{
	if (initialized)
		return;

	kiero::bind(42, (void**)&oEndScene, hkEndScene);
	kiero::bind(16, (void**)&oReset, hkReset);
	initialized = true;
}

void Backends::DX9::Shutdown()
{
	if (!initialized)
		return;

	kiero::shutdown();

	ImGui_ImplDX9_Shutdown();
	Backends::Win32::Shutdown();

	Backends::Win32::Shutdown();
	Backends::ShutdownImGui();

	hookInit = false;
	initialized = false;
}
#endif