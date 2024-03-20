using System.Runtime.InteropServices;

namespace SharpGUI;

internal class SharpGUINatives
{

	[DllImport("SharpGUINative.dll")]
	internal static extern bool InitializeSharpGUI();

	[DllImport("SharpGUINative.dll")]
	internal static extern bool ShutdownSharpGUI();

	[DllImport("SharpGUINative.dll")]
	internal static extern bool SetInitImGuiCallback(IntPtr initImGuiCallback);

	[DllImport("SharpGUINative.dll")]
	internal static extern bool SetRenderCallback(IntPtr renderCallback);

}
