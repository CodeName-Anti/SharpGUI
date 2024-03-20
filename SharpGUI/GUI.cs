namespace SharpGUI;

public class GUI
{

	public static event Action OnInitImGui;
	public static event Action OnRender;

	public static void Initialize()
	{
		GUIHelper.LoadLibraries();

		SharpGUINatives.SetInitImGuiCallback(GUIHelper.CreateNativeDelegate(InitImGuiCallback));
		SharpGUINatives.SetRenderCallback(GUIHelper.CreateNativeDelegate(RenderCallback));

		SharpGUINatives.InitializeSharpGUI();
	}

	private static void InitImGuiCallback()
	{
		OnInitImGui?.Invoke();
	}

	private static void RenderCallback()
	{
		OnRender?.Invoke();
	}

	public static void Shutdown()
	{
		SharpGUINatives.ShutdownSharpGUI();
	}
}
