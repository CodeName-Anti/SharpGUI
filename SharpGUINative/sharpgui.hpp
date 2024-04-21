#pragma once

#include "backends/backends.hpp"

namespace SharpGUI
{
	Backends::BackendType::Enum GetBackendType();

	Backends::BackendType::Enum GetCurrentBackend();

	bool Initialize(Backends::BackendType::Enum backendType);
	bool Initialize();

	bool Shutdown();
}