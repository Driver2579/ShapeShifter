// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActivatableLinesGeneratorCommands.h"

#define LOCTEXT_NAMESPACE "FActivatableLinesGeneratorModule"

void FActivatableLinesGeneratorCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "ActivatableLinesGenerator", "Execute ActivatableLinesGenerator action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
