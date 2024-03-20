#pragma once

#include "dx9_backend.hpp"
#include "dx11_backend.hpp"
#include "win32_backend.hpp"
#include "opengl_backend.hpp"

namespace Backends
{
	void InitImGui();
	void ShutdownImGui();

	void RenderGUI();
}