// Tumult — https://github.com/KirkDiggler/tumult (vendored at v0.1.0)
// rpgkit — https://github.com/KirkDiggler/rpgkit (vendored at v0.3.0, header-only)

using System.IO;
using UnrealBuildTool;

public class Tumult : ModuleRules
{
	public Tumult(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(new string[] { "Core" });

		PublicIncludePaths.Add(ModuleDirectory);
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "rpgkit", "core", "include"));

		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "src"));
	}
}
