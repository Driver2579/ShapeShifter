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

		// UMG
		PrivateDependencyModuleNames.AddRange(new string[] { "UMG" });

		// Navigation for walls
		PrivateDependencyModuleNames.AddRange(new string[] { "SpiderNavigation" });

		// EditorSubsystem is needed only in the editor
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] { "EditorSubsystem", "UnrealEd" });
		}
	}
}