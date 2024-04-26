#include "sharpconfig.hpp"

#if SHARPGUI_INCLUDE_DX9

#include "backends.hpp"
#include "dx9_backend.hpp"
#include "win32_backend.hpp"

#include "d3d9.h"
#include "kiero.h"

#define CIMGUI_USE_DX9
#include "cimgui/cimgui_impl.h"

typedef HRESULT(__stdcall* EndSceneFunc)(IDirect3DDevice9* pDevice);
typedef HRESULT(__stdcall* ResetFunc)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);

namespace Backends::DX9
{
	bool hookInit = false;

	EndSceneFunc oEndScene;
	ResetFunc oReset;
}

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

	return Backends::DX9::oEndScene(pDevice);
}

HRESULT __stdcall hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	HRESULT hr = Backends::DX9::oReset(pDevice, pPresentationParameters);

	ImGui_ImplDX9_CreateDeviceObjects();

	return hr;
}

Backends::BackendType Backends::DX9Backend::GetType()
{
	return Backends::BackendType_DX9;
}

void Backends::DX9Backend::InitializeBackend()
{
	kiero::init(kiero::RenderType::D3D9);
	kiero::bind(42, (void**)&Backends::DX9::oEndScene, hkEndScene);
	kiero::bind(16, (void**)&Backends::DX9::oReset, hkReset);
}

void Backends::DX9Backend::ShutdownBackend()
{
	kiero::shutdown();

	ImGui_ImplDX9_Shutdown();
	Backends::Win32::Shutdown();

	Backends::Win32::Shutdown();
	Backends::ShutdownImGui();
}
#endif