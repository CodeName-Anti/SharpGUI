using BepInEx.Logging;
using BepInEx.Unity.IL2CPP;
using BepInEx;

namespace SharpGUIPlugin;

[BepInPlugin(MyPluginInfo.PLUGIN_GUID, MyPluginInfo.PLUGIN_NAME, MyPluginInfo.PLUGIN_VERSION)]
public class SharpGUIPlugin : BasePlugin
{
	internal static new ManualLogSource Log;

	public override void Load()
	{
		// Plugin startup logic
		Log = base.Log;

		AddComponent<SharpGUIBehaviour>();

		Log.LogInfo($"Plugin {MyPluginInfo.PLUGIN_GUID} is loaded!");
	}
}