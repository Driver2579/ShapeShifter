// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShapeShifterTarget : TargetRules
{
	public ShapeShifterTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ShapeShifter");
	}
}