// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class TumultUE : ModuleRules
{
	public TumultUE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		string TumultRoot = Environment.GetEnvironmentVariable("TUMULT_ROOT");
		if (string.IsNullOrWhiteSpace(TumultRoot))
		{
			TumultRoot = Path.GetFullPath(Path.Combine(
				ModuleDirectory,
				"..",
				"..",
				"..",
				"..",
				"tumult"));
		}

		string TumultIncludePath = Path.Combine(TumultRoot, "include");
		if (!Directory.Exists(TumultIncludePath))
		{
			throw new BuildException(
				"Tumult headers not found at '{0}'. Place tumult beside tumult-ue or set TUMULT_ROOT to the Tumult repository root.",
				TumultIncludePath);
		}

		PrivateIncludePaths.Add(TumultIncludePath);

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
