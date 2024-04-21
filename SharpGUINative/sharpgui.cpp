#include "sharpgui.hpp"
#include "exports.hpp"

#include "sharpconfig.hpp"
#include "kiero.h"
#include "backends/backends.hpp"

namespace SharpGUI
{
	Backends::BackendType::Enum currentBackend;
}

Backends::BackendType::Enum SharpGUI::GetBackendType()
{
	Backends::BackendType::Enum type = Backends::BackendType::None;


#define CHECK_FOR_DLL(dll, backendEnum)				\
	if (GetModuleHandle(TEXT(dll)) != NULL)			\
	{												\
		type = backendEnum;							\
	}

#if SHARPGUI_INCLUDE_OPENGL
	CHECK_FOR_DLL("opengl32.dll", Backends::BackendType::OpenGL);
#endif

#if SHARPGUI_INCLUDE_DX9
	CHECK_FOR_DLL("d3d9.dll", Backends::BackendType::DX9);
#endif

#if SHARPGUI_INCLUDE_DX11
	CHECK_FOR_DLL("d3d11.dll", Backends::BackendType::DX11);
#endif

#undef CHECK_FOR_DLL

#if SHARPGUI_INCLUDE_OVERLAY
	if (type == Backends::BackendType::None)
	{
		type = Backends::BackendType::Overlay;
	}
#endif

	return type;
}

Backends::BackendType::Enum SharpGUI::GetCurrentBackend()
{
	return SharpGUI::currentBackend;
}

bool SharpGUI::Initialize(Backends::BackendType::Enum backendType)
{
	switch (backendType)
	{
#if SHARPGUI_INCLUDE_DX9
	case Backends::BackendType::DX9:
		Backends::DX9::Initialize();
		break;
#endif

#if SHARPGUI_INCLUDE_DX11
	case Backends::BackendType::DX11:
		Backends::DX11::Initialize();
		break;
#endif

#if SHARPGUI_INCLUDE_OPENGL
	case Backends::BackendType::OpenGL:
		Backends::OpenGL::Initialize();
		break;
#endif

#if SHARPGUI_INCLUDE_OVERLAY
	case Backends::BackendType::Overlay:
		Backends::Overlay::Initialize();
		break;
#endif

	default:
		return false;
	}

	currentBackend = backendType;

	return true;
}

bool SharpGUI::Initialize()
{
	return Initialize(GetBackendType());
}

bool SharpGUI::Shutdown()
{
	Backends::BackendType::Enum backendType = GetBackendType();

	switch (backendType)
	{
#if SHARPGUI_INCLUDE_DX9
	case Backends::BackendType::DX9:
		Backends::DX9::Shutdown();
#endif

#if SHARPGUI_INCLUDE_DX11
	case Backends::BackendType::DX11:
		Backends::DX11::Shutdown();
		break;
#endif

#if SHARPGUI_INCLUDE_OPENGL
	case Backends::BackendType::OpenGL:
		Backends::OpenGL::Shutdown();
		break;
#endif

#if SHARPGUI_INCLUDE_OVERLAY
	case Backends::BackendType::Overlay:
		Backends::Overlay::Shutdown();
		break;
#endif

	default:
		return false;
	}

	return true;
}

