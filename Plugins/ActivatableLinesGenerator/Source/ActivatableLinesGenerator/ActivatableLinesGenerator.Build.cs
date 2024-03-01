// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ActivatableLinesGenerator : ModuleRules
{
	public ActivatableLinesGenerator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"ShapeShifter", // Main project
				"SpiderNavigation" // Navigation for walls
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"
			}
			);
	}
}
