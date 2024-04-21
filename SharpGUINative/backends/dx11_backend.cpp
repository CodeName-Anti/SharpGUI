#include "sharpconfig.hpp"

#if SHARPGUI_INCLUDE_DX11

#include "backends.hpp"
#include "d3d11.h"
#include "kiero.h"

#define CIMGUI_USE_DX11
#include "cimgui/cimgui_impl.h"

typedef HRESULT(__stdcall *PresentFunc) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT(__stdcall *ResizeBuffersFunc) (IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

namespace Backends::DX11
{
	bool initialized = false;
	bool hookInit = false;

	PresentFunc oPresent;
	ResizeBuffersFunc oResizeBuffers;

	ID3D11Device* pDevice;
	ID3D11DeviceContext* pContext;
	ID3D11RenderTargetView* pMainRenderTargetView;

	HWND outputWindow;
}

void CreateMainRenderTargetView(IDXGISwapChain* pSwapChain)
{
	ID3D11Texture2D* pBackBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

	if (pBackBuffer == NULL)
	{
		// Shouldnt happen
		__debugbreak();
		return;
	}

	Backends::DX11::pDevice->CreateRenderTargetView(pBackBuffer, NULL, &Backends::DX11::pMainRenderTargetView);
	pBackBuffer->Release();
}

static HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!Backends::DX11::hookInit)
	{
		// Return if we can't get the device
		if (!SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Backends::DX11::pDevice)))
		{
			return Backends::DX11::oPresent(pSwapChain, SyncInterval, Flags);
		}

		// Get device context
		Backends::DX11::pDevice->GetImmediateContext(&Backends::DX11::pContext);

		// Get swapchain description
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		pSwapChain->GetDesc(&swapChainDesc);

		// Get output window from swapchain description
		Backends::DX11::outputWindow = swapChainDesc.OutputWindow;

		// Create new render target view
		CreateMainRenderTargetView(pSwapChain);

		Backends::InitImGui();
		Backends::Win32::Initialize(Backends::DX11::outputWindow);
		ImGui_ImplDX11_Init(Backends::DX11::pDevice, Backends::DX11::pContext);

		Backends::DX11::hookInit = true;
	}

	Backends::Win32::NewFrame();
	ImGui_ImplDX11_NewFrame();
	igNewFrame();

	Backends::RenderGUI();

	igEndFrame();
	igRender();

	Backends::DX11::pContext->OMSetRenderTargets(1, &Backends::DX11::pMainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(igGetDrawData());
	return Backends::DX11::oPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	if (Backends::DX11::pMainRenderTargetView)
	{
		Backends::DX11::pContext->OMSetRenderTargets(0, NULL, NULL);
		Backends::DX11::pMainRenderTargetView->Release();
	}

	HRESULT hr = Backends::DX11::oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	CreateMainRenderTargetView(pSwapChain);

	Backends::DX11::pContext->OMSetRenderTargets(1, &Backends::DX11::pMainRenderTargetView, NULL);

	return hr;
}

void Backends::DX11::Initialize()
{
	if (initialized)
		return;
	
	kiero::init(kiero::RenderType::D3D11);

	kiero::bind(8, (void**)&oPresent, hkPresent);
	kiero::bind(13, (void**)&oResizeBuffers, hkResizeBuffers);

	initialized = true;
}

void Backends::DX11::Shutdown()
{
	if (!initialized)
		return;

	kiero::shutdown();
	
	ImGui_ImplDX11_Shutdown();
	Backends::Win32::Shutdown();

	pDevice->Release();
	pContext->Release();
	pMainRenderTargetView->Release();

	Backends::ShutdownImGui();


	hookInit = false;
	initialized = false;
}
#endif