#pragma once

#include "sharpconfig.hpp"

#if SHARPGUI_INCLUDE_DX9

namespace Backends::DX9
{
	extern bool initialized;

	void Initialize();
	void Shutdown();
}

#endif