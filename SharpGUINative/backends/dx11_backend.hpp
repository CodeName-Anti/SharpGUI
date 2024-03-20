#pragma once

#include "sharpconfig.hpp"

#if SHARPGUI_INCLUDE_DX11

namespace Backends::DX11
{
	extern bool initialized;

	void Initialize();
	void Shutdown();
}

#endif