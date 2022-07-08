// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlushGame : ModuleRules
{
	public PlushGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "OnlineSubsystem", "OnlineSubsystemSteam", "OnlineSubsystemUtils", "Steamworks" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Noesis", "NoesisRuntime" });
	}
}
