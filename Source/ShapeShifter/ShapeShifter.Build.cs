// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShapeShifter : ModuleRules
{
	public ShapeShifter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		
		// Enhanced input
		PublicDependencyModuleNames.AddRange(new string[] { "EnhancedInput" });
		
		// Physical Materials
		PublicDependencyModuleNames.AddRange(new string[] { "PhysicsCore" });
		
		// Niagara
		PublicDependencyModuleNames.AddRange(new string[] { "Niagara" });

		// Water
		PrivateDependencyModuleNames.AddRange(new string[] { "Water" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
