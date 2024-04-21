#pragma once

#include "dx9_backend.hpp"
#include "dx11_backend.hpp"
#include "win32_backend.hpp"
#include "opengl_backend.hpp"

#include "overlay_backend.hpp"

namespace Backends
{
	extern bool handleInput;

	void InitImGui();
	void ShutdownImGui();

	void SetHandleInput(bool handleInput);

	void RenderGUI();

	struct BackendType
	{
		enum Enum
		{
			None,
			DX9,
			DX11,
			OpenGL,
			Overlay
		};
	};
}