# SharpGUI
![GitHub Repo stars](https://img.shields.io/github/stars/CodeName-Anti/SharpGUI?style=for-the-badge)
![GitHub License](https://img.shields.io/github/license/CodeName-Anti/SharpGUI?style=for-the-badge)

Load [Dear ImGui](https://github.com/ocornut/imgui) into any Application.

## Supported Backends
- DX9
- DX11
- OpenGL 2
- OpenGL 3
- Overlay renderer (universal)

## Features
- Intercept input
- Unlock mouse cursor
- Easy to use C# API

## How does it work?
SharpGUI uses custom builds of [cimgui](https://github.com/cimgui/cimgui) and [ImGui.NET](https://github.com/ImGuiNET/ImGui.NET) which include all backends. SharpGUINative contains all the code for hooking and rendering the imgui backends. The C# side of the application invokes exported methods from SharpGUINative.

*Note: SharpGUINative could also be used without C#.*

## Usage
Reference SharpGUI and the custom version of ImGui.NET(can be found in [SharpGUI/Resources](SharpGUI/Resources)) in your C# project. An example for a BepInEx 6 IL2CPP Plugin can be found in the [SharpGUI BepInEx Plugin Directory](SharpGUI%20BepInEx%20Plugin).

## Change backends
Go into [sharpconfig.hpp](SharpGUINative/sharpconfig.hpp) and select the backends you want to include.
### Example: Remove DX11 backend
```cpp
#define SHARPGUI_INCLUDE_DX11		1
```
should be changed into:
```cpp
#define SHARPGUI_INCLUDE_DX11		0
```

## Used Libraries
- [cimgui/cimgui](https://github.com/cimgui/cimgui)
- [ImGuiNET/ImGui.NET](https://github.com/ImGuiNET/ImGui.NET)
- [Rebzzel/kiero](https://github.com/Rebzzel/kiero)
- [TsudaKageyu/minhook](https://github.com/TsudaKageyu/minhook)
- [BepInEx/BepInEx](https://github.com/BepInEx/BepInEx) - used for the [SharpGUI BepInEx Example Plugin](/SharpGUI%20BepInEx%20Plugin)