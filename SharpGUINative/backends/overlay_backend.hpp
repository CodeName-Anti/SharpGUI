#pragma once

#include "sharpconfig.hpp"

#if SHARPGUI_INCLUDE_OVERLAY

namespace Backends::Overlay
{
	extern bool initialized;

	void Initialize(HWND targetWindow);
	void Initialize();
	void Shutdown();
}

#endif