// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShapeShifterEditorTarget : TargetRules
{
	public ShapeShifterEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ShapeShifter");
	}
}