#include "exports.hpp"

#include "sharpconfig.hpp"
#include "kiero.h"
#include "backends/backends.hpp"

namespace CSharpInterop
{
	CSharpCallback initImGuiCallback;
	CSharpCallback renderCallback;
}

bool __stdcall InitializeSharpGUI()
{
    kiero::Status::Enum status = kiero::init(kiero::RenderType::Auto);
    if (status != kiero::Status::Success && status != kiero::Status::AlreadyInitializedError)
    {
        return false;
    }

    kiero::RenderType::Enum renderType = kiero::getRenderType();

    switch (renderType)
    {
#if SHARPGUI_INCLUDE_DX11
    case kiero::RenderType::D3D11:
        Backends::DX11::Initialize();
        break;
#endif

#if SHARPGUI_INCLUDE_OPENGL
    case kiero::RenderType::OpenGL:
        Backends::OpenGL::Initialize();
        break;
#endif

#if SHARPGUI_INCLUDE_DX9
    case kiero::RenderType::D3D9:
        Backends::DX9::Initialize();
#endif
    }

    return true;
}

bool __stdcall ShutdownSharpGUI()
{
    kiero::RenderType::Enum renderType = kiero::getRenderType();

    switch (renderType)
    {
#if SHARPGUI_INCLUDE_DX11
    case kiero::RenderType::D3D11:
        Backends::DX11::Shutdown();
        break;
#endif

#if SHARPGUI_INCLUDE_OPENGL
    case kiero::RenderType::OpenGL:
        Backends::OpenGL::Shutdown();
        break;
#endif

#if SHARPGUI_INCLUDE_DX9
    case kiero::RenderType::D3D9:
        Backends::DX9::Shutdown();
#endif

    default:
        break;
    }

    return true;
}

void __stdcall SetInitImGuiCallback(CSharpInterop::CSharpCallback initImGuiCallback)
{
	CSharpInterop::initImGuiCallback = initImGuiCallback;
}

void __stdcall SetRenderCallback(CSharpInterop::CSharpCallback renderCallback)
{
	CSharpInterop::renderCallback = renderCallback;
}
