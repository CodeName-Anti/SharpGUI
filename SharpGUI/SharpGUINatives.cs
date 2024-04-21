using System.Runtime.InteropServices;

namespace SharpGUI;

internal class SharpGUINatives
{

	[DllImport("SharpGUINative.dll")]
	internal static extern bool InitializeSharpGUI();
	
	[DllImport("SharpGUINative.dll")]
	internal static extern bool InitializeSharpGUIBackend(BackendType backendType);

	[DllImport("SharpGUINative.dll")]
	internal static extern bool ShutdownSharpGUI();

	[DllImport("SharpGUINative.dll")]
	internal static extern void SetInitImGuiCallback(IntPtr initImGuiCallback);

	[DllImport("SharpGUINative.dll")]
	internal static extern void SetRenderCallback(IntPtr renderCallback);

	[DllImport("SharpGUINative.dll")]
	internal static extern void SetHandleInput([MarshalAs(UnmanagedType.Bool)] bool handleInput);

}
