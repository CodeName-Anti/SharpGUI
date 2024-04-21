using CommandLine;
using CommandLine.Text;
using Reloaded.Injector;
using System.Diagnostics;

namespace TestSharp;

internal class Program
{
	public class Options
	{
		[Option("process", Required = true, HelpText = "Process to inject into.")]
		public string Process { get; set; }

		[Option("cimgui", Required = true, HelpText = "Path to cimgui.dll.")]
		public string CimguiPath { get; set; }

		[Option("sharpguinative", Required = true, HelpText = "Path to SharpGUINative.dll.")]
		public string SharpGUINativePath { get; set; }

	}

	private static void Inject(string process, string cimguiPath, string sharpGUINativePath)
	{
		Process[] processes = Process.GetProcessesByName(process);
		if (processes.Length == 0)
		{
			Console.WriteLine(process);
			Console.WriteLine("process not found");
			return;
		}

		Injector injector = new(processes.FirstOrDefault());

		long injectCimgui = injector.Inject(cimguiPath);
		long injectSharpGUINative = injector.Inject(sharpGUINativePath);

		if (injectCimgui == 0)
			throw new Exception("cimgui inject failed");

		if (injectSharpGUINative == 0)
			throw new Exception("SharpGUINative inject failed");

		injector.CallFunction("SharpGUINative.dll", "InitializeSharpGUI", 0);

		Console.WriteLine("injected");
	}

	static void RunOptions(Options opts)
	{
		if (opts.Process.EndsWith(".exe"))
			opts.Process = opts.Process[..^4];

		Inject(opts.Process, opts.CimguiPath, opts.SharpGUINativePath);
	}

	static void Main(string[] args)
	{
		ParserResult<Options> parserResult = new Parser(x =>
		{
			x.AutoHelp = false;
			x.AutoVersion = false;
			x.EnableDashDash = true;
		}).ParseArguments<Options>(args);

		parserResult
			.WithParsed(RunOptions)
			.WithNotParsed(x =>
			{
				var helpText = HelpText.AutoBuild(parserResult, h =>
				{
					h.AutoVersion = false;  // hides --version
					return HelpText.DefaultParsingErrorsHandler(parserResult, h);
				}, e => e);
				Console.WriteLine(helpText);
			});
	}
}
