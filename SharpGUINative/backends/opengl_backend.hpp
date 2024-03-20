#pragma once

#include "sharpconfig.hpp"

#if SHARPGUI_INCLUDE_OPENGL

namespace Backends::OpenGL
{
	extern bool initialized;

	void Initialize();
	void Shutdown();
}

#endif